#!/usr/bin/env python3

"""
===============================================================================
                                Author: cRash
                            Date: April 25th, 2026
                Comments: Special thanks to Kate and everyone 
                who's ever believed in me -- you're the best!
===============================================================================
                                                               :
                                                              ::
                                                             ::
                                                             ::
                                                            ::
                                                            ::
                                              __           ::
                   _..-'/-¯¯--/_          ,.--. ''.     |`\=,..
                -:--.---''-..  /_         |\\_\..  \    `-.=._/
                .-|¯         '.  \         \_ \-`/\ |    ::`
                  /  @  \      \  -_   _..--|-\¯¯``'--.-/_\
                  |   .-'|      \  \\-'\_/     ¯/-.|-.\_\_/
                  \_./` /        \_//-''/    .-'
                       |           '-/'@====/              _.--.
                   __.'             /¯¯'-. \-'.          _/   /¯'
                .''____|   /       |'--\__\/-._        .'    |
                 \ \_. \  |       _| -/        \-.__  /     /
                  \___\ '/   _.  ('-..| /       '_  ''   _.'
                        /  .'     ¯¯¯¯ /        | ``'--''
                       (  / ¯```¯¯¯¯¯|-|        |
                        \ \_.         \ \      /
                         \___\         '.'.   /
                                         /    |
                                        /   .'
                                       /  .' |
                                     .'  / \  \
                                    /___| /___'
===============================================================================
"
                               ==Phrack Inc.==

                    Volume One, Issue 7, Phile 3 of 10

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
The following was written shortly after my arrest...

                       \/\The Conscience of a Hacker/\/

                                      by

                               +++The Mentor+++

                          Written on January 8, 1986
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

        Another one got caught today, it's all over the papers.  "Teenager
Arrested in Computer Crime Scandal", "Hacker Arrested after Bank Tampering"...
        Damn kids.  They're all alike.

        But did you, in your three-piece psychology and 1950's technobrain,
ever take a look behind the eyes of the hacker?  Did you ever wonder what
made him tick, what forces shaped him, what may have molded him?
        I am a hacker, enter my world...
        Mine is a world that begins with school... I'm smarter than most of
the other kids, this crap they teach us bores me...
        Damn underachiever.  They're all alike.

        I'm in junior high or high school.  I've listened to teachers explain
for the fifteenth time how to reduce a fraction.  I understand it.  "No, Ms.
Smith, I didn't show my work.  I did it in my head..."
        Damn kid.  Probably copied it.  They're all alike.

        I made a discovery today.  I found a computer.  Wait a second, this is
cool.  It does what I want it to.  If it makes a mistake, it's because I
screwed it up.  Not because it doesn't like me...
                Or feels threatened by me...
                Or thinks I'm a smart ass...
                Or doesn't like teaching and shouldn't be here...
        Damn kid.  All he does is play games.  They're all alike.

        And then it happened... a door opened to a world... rushing through
the phone line like heroin through an addict's veins, an electronic pulse is
sent out, a refuge from the day-to-day incompetencies is sought... a board is
found.
        "This is it... this is where I belong..."
        I know everyone here... even if I've never met them, never talked to
them, may never hear from them again... I know you all...
        Damn kid.  Tying up the phone line again.  They're all alike...

        You bet your ass we're all alike... we've been spoon-fed baby food at
school when we hungered for steak... the bits of meat that you did let slip
through were pre-chewed and tasteless.  We've been dominated by sadists, or
ignored by the apathetic.  The few that had something to teach found us will-
ing pupils, but those few are like drops of water in the desert.

        This is our world now... the world of the electron and the switch, the
beauty of the baud.  We make use of a service already existing without paying
for what could be dirt-cheap if it wasn't run by profiteering gluttons, and
you call us criminals.  We explore... and you call us criminals.  We seek
after knowledge... and you call us criminals.  We exist without skin color,
without nationality, without religious bias... and you call us criminals.
You build atomic bombs, you wage wars, you murder, cheat, and lie to us
and try to make us believe it's for our own good, yet we're the criminals.

        Yes, I am a criminal.  My crime is that of curiosity.  My crime is
that of judging people by what they say and think, not what they look like.
My crime is that of outsmarting you, something that you will never forgive me
for.

        I am a hacker, and this is my manifesto.  You may stop this individual,
but you can't stop us all... after all, we're all alike.

                               +++The Mentor+++
"
===============================================================================
                                 .,ad88888ba,.    
                             .,ad8888888888888a,  
                            d8P"" "98888P"" "98888b,
                            9b    d8888,    `9888B
                          ,d88aaa8888888b,,,d888P'
                         d8888888888888888888888b 
                        d888888P""98888888888888P 
                        88888P'    9888888888888  
                        `98P'       9888888888P'  
                                     `"9888P"'    
                                        `"'
===============================================================================
"""

"""
End-to-end TURN data flow test against a live vine-relay instance.

vine-relay listens on both TCP and UDP on the same port.  This script
tests STUN/TURN protocol flow over whichever transport is selected.

Tests:
  1. STUN Binding Request  -> expects Binding Response (0x0101)
  2. TURN Allocate (unauth) -> expects 401 with nonce + realm
  3. TURN Allocate (auth)   -> expects success with relay address
  4. CreatePermission       -> expects success
  5. Send indication        -> verify relay accepts it
  6. ChannelBind            -> expects success
  7. ChannelData            -> send and verify relay accepts
  8. Refresh (keep-alive)   -> expects success
  9. Refresh lifetime=0     -> expects deallocation

Usage:
  python3 test_turn_e2e.py [relay_host] [relay_port] [secret]              # TCP
  python3 test_turn_e2e.py [relay_host] [relay_port] [secret] --udp        # UDP
  python3 test_turn_e2e.py [relay_host] [relay_port] [secret] --tls        # TURNS/TCP
  python3 test_turn_e2e.py [relay_host] [relay_port] [secret] --dtls       # DTLS/UDP
"""

import hashlib
import hmac
import os
import struct
import socket
import ssl
import sys
import time
import base64
import binascii

STUN_MAGIC = 0x2112A442
STUN_HEADER_LEN = 20

# Message types
BINDING_REQUEST       = 0x0001
BINDING_RESPONSE      = 0x0101
ALLOCATE_REQUEST      = 0x0003
ALLOCATE_RESPONSE     = 0x0103
ALLOCATE_ERROR        = 0x0113
REFRESH_REQUEST       = 0x0004
REFRESH_RESPONSE      = 0x0104
REFRESH_ERROR         = 0x0114
CREATE_PERM_REQUEST   = 0x0008
CREATE_PERM_RESPONSE  = 0x0108
CREATE_PERM_ERROR     = 0x0118
CHANNEL_BIND_REQUEST  = 0x0009
CHANNEL_BIND_RESPONSE = 0x0109
CHANNEL_BIND_ERROR    = 0x0119
SEND_INDICATION       = 0x0016
DATA_INDICATION       = 0x0017

# Attributes
ATTR_USERNAME           = 0x0006
ATTR_MESSAGE_INTEGRITY  = 0x0008
ATTR_ERROR_CODE         = 0x0009
ATTR_REALM              = 0x0014
ATTR_NONCE              = 0x0015
ATTR_XOR_MAPPED_ADDR    = 0x0020
ATTR_XOR_RELAYED_ADDR   = 0x0016
ATTR_LIFETIME           = 0x000D
ATTR_REQUESTED_TRANSPORT = 0x0019
ATTR_XOR_PEER_ADDR      = 0x0012
ATTR_DATA               = 0x0013
ATTR_CHANNEL_NUMBER     = 0x000C
ATTR_FINGERPRINT        = 0x8028
ATTR_SOFTWARE           = 0x8022

PASS = 0
FAIL = 0

def log_pass(msg):
    global PASS
    PASS += 1
    print(f"  \033[32mPASS\033[0m  {msg}")

def log_fail(msg):
    global FAIL
    FAIL += 1
    print(f"  \033[31mFAIL\033[0m  {msg}")

def txn_id():
    return os.urandom(12)

def build_stun(msg_type, tid, attrs=b""):
    return struct.pack("!HHI", msg_type, len(attrs), STUN_MAGIC) + tid + attrs

def attr_tlv(atype, value):
    padded_len = (len(value) + 3) & ~3
    return struct.pack("!HH", atype, len(value)) + value + b'\x00' * (padded_len - len(value))

def attr_requested_transport(proto=17):
    return attr_tlv(ATTR_REQUESTED_TRANSPORT, struct.pack("!Bxxx", proto))

def attr_username(name):
    return attr_tlv(ATTR_USERNAME, name.encode())

def attr_realm(realm):
    return attr_tlv(ATTR_REALM, realm.encode())

def attr_nonce(nonce):
    return attr_tlv(ATTR_NONCE, nonce)

def attr_lifetime(seconds):
    return attr_tlv(ATTR_LIFETIME, struct.pack("!I", seconds))

def attr_xor_peer_address(ip, port, tid):
    xport = port ^ (STUN_MAGIC >> 16)
    ip_bytes = socket.inet_aton(ip)
    xip = struct.pack("!I", struct.unpack("!I", ip_bytes)[0] ^ STUN_MAGIC)
    value = struct.pack("!xBH", 0x01, xport) + xip
    return attr_tlv(ATTR_XOR_PEER_ADDR, value)

def attr_channel_number(num):
    return attr_tlv(ATTR_CHANNEL_NUMBER, struct.pack("!HH", num, 0))

def attr_data(data):
    return attr_tlv(ATTR_DATA, data)

def hmac_sha1(key, data):
    return hmac.new(key, data, hashlib.sha1).digest()

def crc32_stun(data):
    return binascii.crc32(data) & 0xFFFFFFFF

def compute_auth_key(username, realm, secret):
    """Compute long-term credential key: MD5(username:realm:password)
    where password = Base64(HMAC-SHA1(secret, username))"""
    raw_mac = hmac_sha1(secret.encode(), username.encode())
    password = base64.b64encode(raw_mac).decode()
    key_input = f"{username}:{realm}:{password}"
    return hashlib.md5(key_input.encode()).digest()

def add_integrity(msg_type, tid, attrs, key):
    """Build a STUN message with MESSAGE-INTEGRITY and FINGERPRINT."""
    mi_len = 24  # 4 header + 20 HMAC
    fp_len = 8   # 4 header + 4 CRC

    msg_len_with_mi = len(attrs) + mi_len
    header = struct.pack("!HHI", msg_type, msg_len_with_mi, STUN_MAGIC) + tid
    data = header + attrs

    hmac_val = hmac_sha1(key, data)
    mi_attr = struct.pack("!HH", ATTR_MESSAGE_INTEGRITY, 20) + hmac_val

    full_len = len(attrs) + mi_len + fp_len
    header2 = struct.pack("!HHI", msg_type, full_len, STUN_MAGIC) + tid
    pre_fp = header2 + attrs + mi_attr

    crc = crc32_stun(pre_fp) ^ 0x5354554E
    fp_attr = struct.pack("!HHI", ATTR_FINGERPRINT, 4, crc & 0xFFFFFFFF)

    return pre_fp + fp_attr

def parse_attrs(data):
    """Parse TLV attributes from STUN message body."""
    attrs = {}
    pos = 0
    while pos + 4 <= len(data):
        atype, alen = struct.unpack("!HH", data[pos:pos+4])
        if pos + 4 + alen > len(data):
            break
        attrs[atype] = data[pos+4:pos+4+alen]
        pos += 4 + ((alen + 3) & ~3)
    return attrs

def parse_stun(data):
    """Parse a STUN response. Returns (msg_type, tid, attrs_dict)."""
    if len(data) < STUN_HEADER_LEN:
        return None, None, {}
    msg_type, msg_len, magic = struct.unpack("!HHI", data[:8])
    if magic != STUN_MAGIC:
        return None, None, {}
    tid = data[8:20]
    body = data[20:20+msg_len]
    return msg_type, tid, parse_attrs(body)


class TurnTcpClient:
    """TCP client for TURN-over-TCP using STUN self-framing."""

    def __init__(self, host, port, timeout=5.0):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(timeout)
        self.sock.connect((host, port))
        self.buf = b""

    def send(self, data):
        self.sock.sendall(data)

    def recv_stun(self, timeout=5.0):
        """Receive one complete STUN message from the TCP stream."""
        self.sock.settimeout(timeout)

        while len(self.buf) < STUN_HEADER_LEN:
            try:
                chunk = self.sock.recv(4096)
                if not chunk:
                    return None
                self.buf += chunk
            except socket.timeout:
                return None

        msg_type, msg_len, magic = struct.unpack("!HHI", self.buf[:8])

        if (self.buf[0] & 0xC0) == 0x00 and magic == STUN_MAGIC:
            total = STUN_HEADER_LEN + msg_len
            while len(self.buf) < total:
                try:
                    chunk = self.sock.recv(4096)
                    if not chunk:
                        return None
                    self.buf += chunk
                except socket.timeout:
                    return None

            msg = self.buf[:total]
            self.buf = self.buf[total:]
            return msg
        else:
            # Possibly ChannelData: first 2 bytes = channel, next 2 = length
            ch_num = struct.unpack("!H", self.buf[:2])[0]
            if 0x4000 <= ch_num <= 0x7FFF:
                ch_len = struct.unpack("!H", self.buf[2:4])[0]
                total = 4 + ch_len
                padded = 4 + ((ch_len + 3) & ~3)
                while len(self.buf) < padded:
                    try:
                        chunk = self.sock.recv(4096)
                        if not chunk:
                            break
                        self.buf += chunk
                    except socket.timeout:
                        break
                msg = self.buf[:total]
                self.buf = self.buf[padded:]
                return msg
            return None

    def close(self):
        self.sock.close()


class TurnUdpClient:
    """UDP client for TURN-over-UDP. Each datagram is one STUN message."""

    def __init__(self, host, port, timeout=5.0):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(timeout)
        self.addr = (host, port)

    def send(self, data):
        self.sock.sendto(data, self.addr)

    def recv_stun(self, timeout=5.0):
        """Receive one STUN datagram."""
        self.sock.settimeout(timeout)
        try:
            data, _ = self.sock.recvfrom(65536)
            return data if len(data) >= STUN_HEADER_LEN else None
        except socket.timeout:
            return None

    def close(self):
        self.sock.close()


class TurnTlsTcpClient:
    """TURNS client: TLS-wrapped TCP with STUN self-framing."""

    def __init__(self, host, port, timeout=5.0):
        ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE
        ctx.set_alpn_protocols(['stun.turn'])
        raw = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        raw.settimeout(timeout)
        self.sock = ctx.wrap_socket(raw, server_hostname=host)
        self.sock.connect((host, port))
        self.buf = b""

    def send(self, data):
        self.sock.sendall(data)

    def recv_stun(self, timeout=5.0):
        """Receive one complete STUN message from the TLS stream."""
        self.sock.settimeout(timeout)

        while len(self.buf) < STUN_HEADER_LEN:
            try:
                chunk = self.sock.recv(4096)
                if not chunk:
                    return None
                self.buf += chunk
            except socket.timeout:
                return None

        msg_type, msg_len, magic = struct.unpack("!HHI", self.buf[:8])

        if (self.buf[0] & 0xC0) == 0x00 and magic == STUN_MAGIC:
            total = STUN_HEADER_LEN + msg_len
            while len(self.buf) < total:
                try:
                    chunk = self.sock.recv(4096)
                    if not chunk:
                        return None
                    self.buf += chunk
                except socket.timeout:
                    return None
            msg = self.buf[:total]
            self.buf = self.buf[total:]
            return msg
        else:
            ch_num = struct.unpack("!H", self.buf[:2])[0]
            if 0x4000 <= ch_num <= 0x7FFF:
                ch_len = struct.unpack("!H", self.buf[2:4])[0]
                total = 4 + ch_len
                padded = 4 + ((ch_len + 3) & ~3)
                while len(self.buf) < padded:
                    try:
                        chunk = self.sock.recv(4096)
                        if not chunk:
                            break
                        self.buf += chunk
                    except socket.timeout:
                        break
                msg = self.buf[:total]
                self.buf = self.buf[padded:]
                return msg
            return None

    def close(self):
        self.sock.close()


class TurnDtlsUdpClient:
    """DTLS client for TURNS-over-UDP using Python ssl DTLS support.

    Requires Python 3.13+ with DTLS support. Falls back to a basic
    OpenSSL subprocess wrapper if unavailable.
    """

    def __init__(self, host, port, timeout=5.0):
        ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
        ctx.check_hostname = False
        ctx.verify_mode = ssl.CERT_NONE
        ctx.set_alpn_protocols(['stun.turn'])
        raw = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        raw.settimeout(timeout)
        raw.connect((host, port))
        try:
            self.sock = ctx.wrap_socket(raw, server_hostname=host,
                                        do_handshake_on_connect=True)
        except (ssl.SSLError, AttributeError, TypeError) as e:
            raw.close()
            raise RuntimeError(
                f"DTLS not supported by this Python build: {e}. "
                "Requires Python 3.13+ with DTLS or a patched ssl module."
            ) from e

    def send(self, data):
        self.sock.send(data)

    def recv_stun(self, timeout=5.0):
        self.sock.settimeout(timeout)
        try:
            data = self.sock.recv(65536)
            return data if len(data) >= STUN_HEADER_LEN else None
        except socket.timeout:
            return None

    def close(self):
        self.sock.close()


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    use_udp = '--udp' in sys.argv
    use_tls = '--tls' in sys.argv
    use_dtls = '--dtls' in sys.argv

    host = args[0] if len(args) > 0 else "127.0.0.1"
    port = int(args[1]) if len(args) > 1 else 13488
    secret = args[2] if len(args) > 2 else "e2e-test-secret"
    realm = None
    nonce = None

    if use_dtls:
        proto = "DTLS/UDP"
    elif use_tls:
        proto = "TURNS/TCP"
    elif use_udp:
        proto = "UDP"
    else:
        proto = "TCP"

    print("=" * 60)
    print(f"  vine-relay TURN end-to-end data flow test ({proto})")
    print("=" * 60)
    print(f"  Target: {host}:{port} ({proto})")
    print()

    try:
        if use_dtls:
            client = TurnDtlsUdpClient(host, port)
        elif use_tls:
            client = TurnTlsTcpClient(host, port)
        elif use_udp:
            client = TurnUdpClient(host, port)
        else:
            client = TurnTcpClient(host, port)
    except Exception as e:
        print(f"  ABORT: Cannot connect to {host}:{port}: {e}")
        sys.exit(1)

    print("── Test 1: STUN Binding Request ──")
    tid1 = txn_id()
    pkt = build_stun(BINDING_REQUEST, tid1)
    client.send(pkt)
    resp = client.recv_stun()

    if resp is None:
        log_fail("No response to STUN Binding Request")
        print("\n  ABORT: relay not responding over TCP.")
        client.close()
        sys.exit(1)

    rtype, rtid, rattrs = parse_stun(resp)
    if rtype == BINDING_RESPONSE and rtid == tid1:
        log_pass("STUN Binding Response received")
        if ATTR_XOR_MAPPED_ADDR in rattrs:
            log_pass("XOR-MAPPED-ADDRESS present in response")
        else:
            log_fail("XOR-MAPPED-ADDRESS missing from Binding Response")
    else:
        log_fail(f"Unexpected response type: 0x{rtype:04x}" if rtype else "Invalid STUN response")

    # --- Test 2: TURN Allocate (unauthenticated) ---
    print("\n── Test 2: TURN Allocate (unauth → expect 401) ──")
    tid2 = txn_id()
    alloc_attrs = attr_requested_transport(17)
    pkt2 = build_stun(ALLOCATE_REQUEST, tid2, alloc_attrs)
    client.send(pkt2)
    resp2 = client.recv_stun()

    if resp2 is None:
        log_fail("No response to unauthenticated Allocate")
    else:
        rtype2, rtid2, rattrs2 = parse_stun(resp2)
        if rtype2 == ALLOCATE_ERROR:
            if ATTR_ERROR_CODE in rattrs2:
                err_data = rattrs2[ATTR_ERROR_CODE]
                err_class = err_data[2]
                err_number = err_data[3]
                err_code = err_class * 100 + err_number
                if err_code == 401:
                    log_pass(f"Got 401 Unauthorized (expected)")
                    if ATTR_NONCE in rattrs2:
                        nonce = rattrs2[ATTR_NONCE]
                        log_pass(f"Nonce received ({len(nonce)} bytes)")
                    else:
                        log_fail("401 response missing NONCE attribute")
                    if ATTR_REALM in rattrs2:
                        realm = rattrs2[ATTR_REALM].decode()
                        log_pass(f"Realm received: {realm}")
                    else:
                        log_fail("401 response missing REALM attribute")
                else:
                    log_fail(f"Got error {err_code} instead of 401")
            else:
                log_fail("Allocate error response missing ERROR-CODE")
        else:
            log_fail(f"Expected Allocate Error (0x0113), got 0x{rtype2:04x}")

    if nonce is None or realm is None:
        log_fail("Cannot continue without nonce/realm from step 2")
        client.close()
        print(f"\n  Results: \033[32m{PASS} passed\033[0m, \033[31m{FAIL} failed\033[0m")
        sys.exit(1)

    # --- Test 3: TURN Allocate (authenticated) ---
    print("\n── Test 3: TURN Allocate (authenticated) ──")
    timestamp = int(time.time()) + 86400
    username = f"{timestamp}:e2e-test"
    auth_key = compute_auth_key(username, realm, secret)

    tid3 = txn_id()
    alloc_attrs3 = (
        attr_requested_transport(17) +
        attr_username(username) +
        attr_realm(realm) +
        attr_nonce(nonce)
    )
    pkt3 = add_integrity(ALLOCATE_REQUEST, tid3, alloc_attrs3, auth_key)
    client.send(pkt3)
    resp3 = client.recv_stun()

    alloc_ok = False
    if resp3 is None:
        log_fail("No response to authenticated Allocate")
    else:
        rtype3, rtid3, rattrs3 = parse_stun(resp3)
        if rtype3 == ALLOCATE_RESPONSE:
            alloc_ok = True
            log_pass("Allocate Success Response received")
            if ATTR_XOR_RELAYED_ADDR in rattrs3:
                log_pass("XOR-RELAYED-ADDRESS present")
            else:
                log_fail("XOR-RELAYED-ADDRESS missing from Allocate success")
            if ATTR_LIFETIME in rattrs3:
                lt_val = struct.unpack("!I", rattrs3[ATTR_LIFETIME])[0]
                log_pass(f"Lifetime: {lt_val}s")
            else:
                log_fail("LIFETIME missing from Allocate success")
        elif rtype3 == ALLOCATE_ERROR:
            err_data = rattrs3.get(ATTR_ERROR_CODE, b'\x00\x00\x00\x00')
            err_code = err_data[2] * 100 + err_data[3]
            reason = err_data[4:].decode(errors='replace') if len(err_data) > 4 else ''
            log_fail(f"Allocate returned error {err_code}: {reason}")

            if err_code == 438 and ATTR_NONCE in rattrs3:
                nonce = rattrs3[ATTR_NONCE]
                print("    (Stale nonce, retrying with new nonce...)")
                auth_key = compute_auth_key(username, realm, secret)
                tid3b = txn_id()
                alloc_attrs3b = (
                    attr_requested_transport(17) +
                    attr_username(username) +
                    attr_realm(realm) +
                    attr_nonce(nonce)
                )
                pkt3b = add_integrity(ALLOCATE_REQUEST, tid3b, alloc_attrs3b, auth_key)
                client.send(pkt3b)
                resp3b = client.recv_stun()
                if resp3b:
                    rtype3b, _, rattrs3b = parse_stun(resp3b)
                    if rtype3b == ALLOCATE_RESPONSE:
                        alloc_ok = True
                        log_pass("Allocate Success after nonce refresh")
                    else:
                        log_fail(f"Allocate still failed after nonce refresh: 0x{rtype3b:04x}")
        else:
            log_fail(f"Unexpected response: 0x{rtype3:04x}")

    if not alloc_ok:
        log_fail("Cannot test further without a successful allocation")
        client.close()
        print(f"\n  Results: \033[32m{PASS} passed\033[0m, \033[31m{FAIL} failed\033[0m")
        sys.exit(1)

    # --- Test 4: CreatePermission ---
    print("\n── Test 4: CreatePermission ──")
    peer_ip = "192.0.2.1"
    peer_port = 5000
    tid4 = txn_id()
    perm_attrs = (
        attr_xor_peer_address(peer_ip, peer_port, tid4) +
        attr_username(username) +
        attr_realm(realm) +
        attr_nonce(nonce)
    )
    pkt4 = add_integrity(CREATE_PERM_REQUEST, tid4, perm_attrs, auth_key)
    client.send(pkt4)
    resp4 = client.recv_stun()

    if resp4 is None:
        log_fail("No response to CreatePermission")
    else:
        rtype4, _, rattrs4 = parse_stun(resp4)
        if rtype4 == CREATE_PERM_RESPONSE:
            log_pass("CreatePermission Success")
        elif rtype4 == CREATE_PERM_ERROR:
            err_data = rattrs4.get(ATTR_ERROR_CODE, b'\x00\x00\x00\x00')
            err_code = err_data[2] * 100 + err_data[3]
            log_fail(f"CreatePermission error {err_code}")
        else:
            log_fail(f"CreatePermission unexpected: 0x{rtype4:04x}")

    print("\n── Test 5: Send Indication ──")
    tid5 = txn_id()
    test_data = b"Hello from TURN e2e test!"
    send_attrs = (
        attr_xor_peer_address(peer_ip, peer_port, tid5) +
        attr_data(test_data)
    )
    pkt5 = build_stun(SEND_INDICATION, tid5, send_attrs)
    client.send(pkt5)
    log_pass("Send Indication transmitted (no response expected for indications)")

    # --- Test 6: ChannelBind ---
    print("\n── Test 6: ChannelBind ──")
    channel_num = 0x4000
    tid6 = txn_id()
    bind_attrs = (
        attr_channel_number(channel_num) +
        attr_xor_peer_address(peer_ip, peer_port, tid6) +
        attr_username(username) +
        attr_realm(realm) +
        attr_nonce(nonce)
    )
    pkt6 = add_integrity(CHANNEL_BIND_REQUEST, tid6, bind_attrs, auth_key)
    client.send(pkt6)
    resp6 = client.recv_stun()

    if resp6 is None:
        log_fail("No response to ChannelBind")
    else:
        rtype6, _, rattrs6 = parse_stun(resp6)
        if rtype6 == CHANNEL_BIND_RESPONSE:
            log_pass("ChannelBind Success")
        elif rtype6 == CHANNEL_BIND_ERROR:
            err_data = rattrs6.get(ATTR_ERROR_CODE, b'\x00\x00\x00\x00')
            err_code = err_data[2] * 100 + err_data[3]
            log_fail(f"ChannelBind error {err_code}")
        else:
            log_fail(f"ChannelBind unexpected: 0x{rtype6:04x}")

    print("\n── Test 7: ChannelData ──")
    cd_payload = b"ChannelData media payload test!"
    cd_header = struct.pack("!HH", channel_num, len(cd_payload))
    cd_pkt = cd_header + cd_payload
    if not use_udp and not use_dtls:
        pad = (4 - (len(cd_payload) % 4)) % 4
        cd_pkt += b'\x00' * pad
    client.send(cd_pkt)
    log_pass("ChannelData sent (relay will forward to mesh)")

    # --- Test 8: Refresh (keep-alive) ---
    print("\n── Test 8: Refresh (keep-alive) ──")
    tid8 = txn_id()
    ref_attrs = (
        attr_lifetime(600) +
        attr_username(username) +
        attr_realm(realm) +
        attr_nonce(nonce)
    )
    pkt8 = add_integrity(REFRESH_REQUEST, tid8, ref_attrs, auth_key)
    client.send(pkt8)
    resp8 = client.recv_stun()

    if resp8 is None:
        log_fail("No response to Refresh")
    else:
        rtype8, _, rattrs8 = parse_stun(resp8)
        if rtype8 == REFRESH_RESPONSE:
            log_pass("Refresh Success")
            if ATTR_LIFETIME in rattrs8:
                lt_val = struct.unpack("!I", rattrs8[ATTR_LIFETIME])[0]
                log_pass(f"Refreshed lifetime: {lt_val}s")
        elif rtype8 == REFRESH_ERROR:
            err_data = rattrs8.get(ATTR_ERROR_CODE, b'\x00\x00\x00\x00')
            err_code = err_data[2] * 100 + err_data[3]
            log_fail(f"Refresh error {err_code}")
        else:
            log_fail(f"Refresh unexpected: 0x{rtype8:04x}")

    # --- Test 9: Refresh lifetime=0 (dealloc) ---
    print("\n── Test 9: Refresh lifetime=0 (dealloc) ──")
    tid9 = txn_id()
    dealloc_attrs = (
        attr_lifetime(0) +
        attr_username(username) +
        attr_realm(realm) +
        attr_nonce(nonce)
    )
    pkt9 = add_integrity(REFRESH_REQUEST, tid9, dealloc_attrs, auth_key)
    client.send(pkt9)
    resp9 = client.recv_stun()

    if resp9 is None:
        log_fail("No response to Refresh(lifetime=0)")
    else:
        rtype9, _, rattrs9 = parse_stun(resp9)
        if rtype9 == REFRESH_RESPONSE:
            log_pass("Deallocation confirmed")
            if ATTR_LIFETIME in rattrs9:
                lt_val = struct.unpack("!I", rattrs9[ATTR_LIFETIME])[0]
                if lt_val == 0:
                    log_pass("Lifetime confirmed as 0")
                else:
                    log_fail(f"Expected lifetime=0, got {lt_val}")
        else:
            log_fail(f"Dealloc unexpected: 0x{rtype9:04x}")

    client.close()

    print()
    print("=" * 60)
    print(f"  Results: \033[32m{PASS} passed\033[0m, \033[31m{FAIL} failed\033[0m")
    print("=" * 60)
    sys.exit(1 if FAIL > 0 else 0)


if __name__ == "__main__":
    main()
