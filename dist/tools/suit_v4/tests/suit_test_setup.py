#!/usr/bin/env python3

import sys
import os
import logging
import subprocess
import argparse
import time
import pexpect
from pexpect import popen_spawn


LOG_HANDLER = logging.StreamHandler()
LOG_HANDLER.setFormatter(logging.Formatter(logging.BASIC_FORMAT))
LOG_LEVELS = ('debug', 'info', 'warning', 'error')

SUIT_DIR = os.path.dirname(sys.argv[0])
RIOT_DIR = os.path.abspath(os.path.join(SUIT_DIR, '../../../..'))
COAPROOT_DIR = os.path.join(RIOT_DIR, 'coaproot')


def get_make_args(jobs, args):
    if jobs is not None:
        make_args = ['-j' + str(jobs)]
    else:
        make_args = ['-j1']
    if args is not None:
        make_args.append(str(args))
    return make_args


def list_from_string(list_str=None):
    value = (list_str or '').split(' ')
    return [v for v in value if v]


def setup_fileserver(cwd_dir):
    logger.info('Setting up aiocoap-fileserver')
    cmd = ['./aiocoap/aiocoap-fileserver', COAPROOT_DIR]
    process = subprocess.Popen(cmd, cwd=os.path.expanduser(cwd_dir),
                               stdout=subprocess.DEVNULL,
                               stderr=subprocess.DEVNULL)
    return process


def setup_ethos(port, prefix, cwd_dir):
    cmd = ['sudo', './dist/tools/ethos/start_network.sh', port, 'suit0', prefix]
    process = subprocess.Popen(cmd, cwd=os.path.expanduser(cwd_dir), preexec_fn=os.setpgrp,
                               stdout=subprocess.DEVNULL,
                               stderr=subprocess.DEVNULL)
    cmd = ['sudo', 'ip', 'address', 'add', prefix, 'dev', 'suit0']
    subprocess.call(cmd, cwd=os.path.expanduser(cwd_dir),
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL)
    return process


def make_delkeys(cwd_dir):
    logger.info('Removing old Keys in {}'.format(cwd_dir))
    cmd = ['rm', '-f', 'public.key', 'public_key.h', 'secret.key']
    subprocess.call(cmd, cwd=os.path.expanduser(cwd_dir),
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL)


def make_genkey(cwd_dir):
    logger.info('Generating keys at {}'.format(cwd_dir))
    cmd = ['make', 'suit/keyhdr']
    subprocess.call(cmd, cwd=os.path.expanduser(cwd_dir),
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL)


def make_reset(board, cwd_dir, port):
    logger.info('Reseting board {}'.format(board))
    cmd = ['make', 'reset', 'BOARD={}'.format(board), 'PORT={}'.format(port)]
    subprocess.call(cmd, cwd=os.path.expanduser(cwd_dir),
                    stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL)


def make_test(board, cwd_dir, port, tags):
    logger.info('Runing tests on {} for application {}'.format(board, cwd_dir))
    cmd = ['make', 'test', 'BOARD={}'.format(board), 'PORT={}'.format(port)]
    if tags is not None:
        cmd.extend(['TAGS={}'.format(' '.join(tags))])
    subprocess.Popen(cmd, cwd=os.path.expanduser(cwd_dir)).wait()


def make_flash(board, cwd_dir, make_args):
    logger.info('Initial Flash of {}'.format(board))
    cmd = ['make', 'clean', 'riotboot/flash', 'BOARD={}'.format(board)]
    cmd.extend(make_args)
    subprocess.Popen(cmd, cwd=os.path.expanduser(cwd_dir)).wait()


def make_publish(board, server_url, cwd_dir, make_args, tag):
    logger.info('Publishing Firmware to %s', server_url)
    cmd = ['make', 'suit/publish', 'BOARD={}'.format(board),
           'SUIT_EXTENSION={}'.format(tag),
           'SUIT_COAP_SERVER={}'.format(server_url)]
    cmd.extend(make_args)
    subprocess.Popen(cmd, cwd=os.path.expanduser(cwd_dir)).wait()


def port_cleanup(port):
    cmd = ['fuser', '-k', port]
    subprocess.call(cmd, stdout=subprocess.DEVNULL,
                    stderr=subprocess.DEVNULL)

PARSER = argparse.ArgumentParser(
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
PARSER.add_argument('app_dir', help='Directory of application to test')
PARSER.add_argument('--board-node', default='samr21-xpro',
                    help='Board to test')
PARSER.add_argument('--board-ethos', default='iotlab-m3',
                    help='Board to test')
PARSER.add_argument('--coap-host', default='[fd00:dead:beef::1]',
                    help='CoAP server host.')
PARSER.add_argument('--ethos', default=False, action='store_true',
                    help='True if test is to be setup locally over ethos.')
PARSER.add_argument('--fileserver', default=False, action='store_true',
                    help='Start aiocoap fileserver, Default=True')
PARSER.add_argument('--jobs', '-j', type=int, default=None,
                    help="Parallel building (0 means no limit, like '--jobs')")
PARSER.add_argument('--keys', default=False, action='store_true',
                    help='Remove old keys and generate new ones, Default True')
PARSER.add_argument('--loglevel', choices=LOG_LEVELS, default='info',
                    help='Python logger log level')
PARSER.add_argument('--make', default=None,
                    help='Additional make arguments')
PARSER.add_argument('--node', default=False, action='store_true',
                    help='Flashes target node , Default=True')
PARSER.add_argument('--port-ethos', default='/dev/ttyUSB1',
                    help='Ethos serial port.')
PARSER.add_argument('--port-node', default='/dev/ttyACM0',
                    help='Node serial port.')
PARSER.add_argument('--prefix', default='fd01::1/64',
                    help='Prefix to propagate over ethos.')
PARSER.add_argument('--publish', default=False, action='store_true',
                    help='Published new Firmware , Default=False')
PARSER.add_argument('--riot_dir', default=RIOT_DIR,
                    help='Base Directory for RIOT')
PARSER.add_argument('--tags', type=list_from_string, default='latest',
                    help='List of manifest tags to publish')
PARSER.add_argument('--test', default=False, action='store_true',
                    help='Runs test for application')


if __name__ == "__main__":
    """For one board test if specified application is updatable"""
    args = PARSER.parse_args()

    logger = logging.getLogger()
    if args.loglevel:
        loglevel = logging.getLevelName(args.loglevel.upper())
        logger.setLevel(loglevel)
    logger.addHandler(LOG_HANDLER)

    app_dir     = args.app_dir
    riot_dir    = args.riot_dir
    board_node  = args.board_node
    board_ethos = args.board_ethos
    host        = args.coap_host
    port_ethos  = args.port_ethos
    port_node   = args.port_node
    prefix      = args.prefix
    make_args   = get_make_args(args.jobs, args.make)
    tags        = args.tags

    childs = []

    try:
        # Setup Ethos
        if args.ethos is True:
            ethos = setup_ethos(port_ethos, prefix, riot_dir)
            childs.append(ethos)
            time.sleep(1)
            logger.info("Ethos pid {} and group pid {}".format(ethos.pid, os.getpgid(ethos.pid)))

        # Setup File Sever
        if args.fileserver is True:
            childs.append(setup_fileserver(SUIT_DIR))

        # Delete old key and generate new ones
        if args.keys is True:
            make_delkeys(app_dir)
            make_genkey(app_dir)

        # Provide node, initial flash
        if args.node is True:
            make_flash(board_node, app_dir, make_args)

        # Publish tags
        if args.publish is True:
            for tag in tags:
                make_publish(board_node, host, app_dir, make_args, tag)

        # Run tests and keep running if fileserver or ethos were setup
        if args.test is True:
            make_test(board_node, app_dir, port_node, tags)
        elif args.ethos is True or args.fileserver is True:
            while True:
                time.sleep(1)

    except SystemExit as e:
        sys.exit(e)

    finally:
        if childs:
            for process in childs:
                try:
                    gpid = os.getpgid(process.pid)
                    logger.info("Killing group {}".format(gpid))
                    subprocess.check_call(["sudo", "kill", '-{}'.format(gpid)])
                except:
                    logger.info("Failed to stop process {}".format(process.pid))
        if args.ethos is True:
            logger.info("Port ethos Cleanup")
            port_cleanup(port_ethos)
        if args.test is True:
            logger.info("Port node Cleanup")
            port_cleanup(port_node)
           
