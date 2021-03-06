<?php

/*
	Copyright (C) 2007 Scott Ullrich
	Copyright (C) 2008 Shrew Soft Inc
	Copyright (C) 2003-2004 Manuel Kasper
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
	AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
	OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/

/* IPsec defines */
$ipsec_loglevels = array("dmn" => "Daemon", "mgr" => "SA Manager", "ike" => "IKE SA", "chd" => "IKE Child SA",
	"job" => "Job Processing", "cfg" => "Configuration backend", "knl" => "Kernel Interface",
	"net" => "Networking", "asn" => "ASN encoding", "enc" => "Message encoding",
	"imc" => "Integrity checker", "imv" => "Integrity Verifier", "pts" => "Platform Trust Service",
	"tls" => "TLS handler", "esp" => "IPsec traffic", "lib" => "StrongSWAN Lib");

$p1_ealgos = array(
	'aes' => array( 'name' => 'AES', 'keysel' => array( 'lo' => 128, 'hi' => 256, 'step' => 64 ) ),
	'aes128gcm' => array( 'name' => 'AES128-GCM', 'keysel' => array( 'lo' => 64, 'hi' => 128, 'step' => 32 ) ),
	'aes192gcm' => array( 'name' => 'AES192-GCM', 'keysel' => array( 'lo' => 64, 'hi' => 128, 'step' => 32 ) ),
	'aes256gcm' => array( 'name' => 'AES256-GCM', 'keysel' => array( 'lo' => 64, 'hi' => 128, 'step' => 32 ) ),
	'blowfish' => array( 'name' => 'Blowfish', 'keysel' => array( 'lo' => 128, 'hi' => 256, 'step' => 64 ) ),
	'3des' => array( 'name' => '3DES' ),
	'cast128' => array( 'name' => 'CAST128' ),
	'des' => array( 'name' => 'DES' ));

$p2_ealgos = array(
	'aes' => array( 'name' => 'AES', 'keysel' => array( 'lo' => 128, 'hi' => 256, 'step' => 64 ) ),
	'aes128gcm' => array( 'name' => 'AES128-GCM', 'keysel' => array( 'lo' => 64, 'hi' => 128, 'step' => 32 ) ),
	'aes192gcm' => array( 'name' => 'AES192-GCM', 'keysel' => array( 'lo' => 64, 'hi' => 128, 'step' => 32 ) ),
	'aes256gcm' => array( 'name' => 'AES256-GCM', 'keysel' => array( 'lo' => 64, 'hi' => 128, 'step' => 32 ) ),
	'blowfish' => array( 'name' => 'Blowfish', 'keysel' => array( 'lo' => 128, 'hi' => 256, 'step' => 64 ) ),
	'3des' => array( 'name' => '3DES' ),
	'cast128' => array( 'name' => 'CAST128' ),
	'des' => array( 'name' => 'DES' ));

$p1_halgos = array(
	'md5' => 'MD5',
	'sha1' => 'SHA1',
	'sha256' => 'SHA256',
	'sha384' => 'SHA384',
	'sha512' => 'SHA512',
	'aesxcbc' => 'AES-XCBC'
);

$p2_halgos = array(
	'hmac_md5' => 'MD5',
	'hmac_sha1' => 'SHA1',
	'hmac_sha256' => 'SHA256',
	'hmac_sha384' => 'SHA384',
	'hmac_sha512' => 'SHA512',
	'aesxcbc' => 'AES-XCBC'
);


$p2_protos = array(
	'esp' => 'ESP',
	'ah' => 'AH');

$p2_pfskeygroups = array(
	0 => 'off',
	1  => '1 (768 bit)',
	2  => '2 (1024 bit)',
	5  => '5 (1536 bit)',
	14 => '14 (2048 bit)',
	15 => '15 (3072 bit)',
	16 => '16 (4096 bit)',
	17 => '17 (6144 bit)',
	18 => '18 (8192 bit)'
);

/*
 * Return phase1 local address
 */
function ipsec_get_phase1_src(& $ph1ent) {
	if (!empty($ph1ent['interface'])) {
		if (!is_ipaddr($ph1ent['interface'])) {
			if (strpos($ph1ent['interface'],'_vip') !== false) {
				// if this is a vip, set the interface to $ph1ent['interface']
				$if = $ph1ent['interface'];
			} else {
				// not a vip, check failover interface
				if ($ph1ent['protocol'] == "inet6") {
						$if = get_failover_interface($ph1ent['interface'], "inet6");
				} else {
						$if = get_failover_interface($ph1ent['interface']);
				}
			}
		} else {
			// interface is an ip address, return
			return $ph1ent['interface'];
		}
	} else {
		$if = "wan";
	}
	if ($ph1ent['protocol'] == "inet6") {
		return get_interface_ipv6($if);
	} else {
		return get_interface_ip($if);
	}
}

/*
 * Return phase1 local address
 */
function ipsec_get_phase1_dst(& $ph1ent) {
	global $g;

	if (empty($ph1ent['remote-gateway']))
		return false;
	$rg = $ph1ent['remote-gateway'];
	if (!is_ipaddr($rg)) {
		if(! file_exists("/var/run/booting"))
			return resolve_retry($rg);
	}
	if(!is_ipaddr($rg))
		return false;

	return $rg;
}

/*
 * Return phase2 idinfo in cidr format
 */
function ipsec_idinfo_to_cidr(& $idinfo, $addrbits = false, $mode = "") {
	global $config;

	switch ($idinfo['type']) {
		case "address":
			if ($addrbits) {
				if ($mode == "tunnel6")
					return $idinfo['address']."/128";
				else
					return $idinfo['address']."/32";
			} else
				return $idinfo['address'];
			break; /* NOTREACHED */
		case "network":
			return "{$idinfo['address']}/{$idinfo['netbits']}";
			break; /* NOTREACHED */
		case "none":
		case "mobile":
			return "0.0.0.0/0";
			break; /* NOTREACHED */
		default:
			if (empty($mode) && !empty($idinfo['mode']))
				$mode = $idinfo['mode'];

			if ($mode == "tunnel6") {
				$address = get_interface_ipv6($idinfo['type']);
				$netbits = get_interface_subnetv6($idinfo['type']);
				$address = gen_subnetv6($address,$netbits);
				return "{$address}/{$netbits}";
			} else {
				$address = get_interface_ip($idinfo['type']);
				$netbits = get_interface_subnet($idinfo['type']);
				$address = gen_subnet($address,$netbits);
				return "{$address}/{$netbits}";
			}
			break; /* NOTREACHED */
	}
}


/*
 *  Return phase2 idinfo in text format
 */
function ipsec_idinfo_to_text(& $idinfo) {
	global $config;

	switch ($idinfo['type']) {
        case "address":
		return $idinfo['address'];
		break; /* NOTREACHED */
        case "network":
		return $idinfo['address']."/".$idinfo['netbits'];
		break; /* NOTREACHED */
	case "mobile":
		return gettext("Mobile Client");
		break; /* NOTREACHED */
	case "none":
		return gettext("None");
		break; /* NOTREACHED */
        default:
		if (!empty($config['interfaces'][$idinfo['type']]))
			return convert_friendly_interface_to_friendly_descr($idinfo['type']);
		else
			return strtoupper($idinfo['type']);
		break; /* NOTREACHED */
	}
}

/*
 * Return phase1 association for phase2
 */
function ipsec_lookup_phase1(& $ph2ent,& $ph1ent) {
	global $config;

	if (!is_array($config['ipsec']))
		return false;
	if (!is_array($config['ipsec']['phase1']))
		return false;
	if (empty($config['ipsec']['phase1']))
		return false;

	foreach ($config['ipsec']['phase1'] as $ph1tmp) {
	    if ($ph1tmp['ikeid'] == $ph2ent['ikeid']) {
		$ph1ent = $ph1tmp;
		return $ph1ent;
	    }
	}

	return false;
}

/*
 * Check phase1 communications status
 */
function ipsec_phase1_status($ipsec_status, $ikeid) {

	foreach ($ipsec_status as $ike) {
		if ($ike['id'] != $ikeid)
			continue;
		if ($ike['status'] == 'established')
			return true;
		break;
	}

	return false;
}

function ipsec_smp_dump_status() {
	global $config, $g, $custom_listtags;

	if (!file_exists("/var/run/charon.xml")) {
		log_error("IPsec daemon seems to have issues or not running!");
		return;
	}

	$fd = @fsockopen("unix:///var/run/charon.xml");
	if (!$fd) {
		log_error("Could not read status from ipsec");
		return;
	}
	$query = '<?xml version="1.0"?><message xmlns="http://www.strongswan.org/smp/1.0" type="request" id="1">';
	$query .= '<query><ikesalist/></query></message>';

	@fwrite($fd, $query);
	$response = "";
	while (!strstr($sread, "</message>")) {
		$sread = fgets($fd);
		$response .= $sread;
	}
	fclose($fd);

	$smp_status_xml = '/tmp/smp_status.xml';
	@file_put_contents($smp_status_xml, $response);
	unset($response, $sread);

	$custom_listtags = array('ikesa', 'childsa', 'network', 'auth');
	$response = parse_xml_config($smp_status_xml, 'message');
	@unlink($smp_status_xml);
	unset($custom_listtags);

	return $response;
}

/*
 * Return dump of SPD table
 */
function ipsec_dump_spd()
{
	$fd = @popen("/sbin/setkey -DP", "r");
	$spd = array();
	if ($fd) {
		while (!feof($fd)) {
			$line = chop(fgets($fd));
			if (!$line)
				continue;
			if ($line == "No SPD entries.")
				break;
			if ($line[0] != "\t") {
				if (is_array($cursp))
					$spd[] = $cursp;
				$cursp = array();
				$linea = explode(" ", $line);
				$cursp['srcid'] = substr($linea[0], 0, strpos($linea[0], "["));
				$cursp['dstid'] = substr($linea[1], 0, strpos($linea[1], "["));
				$i = 0;
			} else if (is_array($cursp)) {
				$linea = explode(" ", trim($line));
				switch($i)
				{
					case 1:
						if ($linea[1] == "none")	/* don't show default anti-lockout rule */
							unset($cursp);
						else
							$cursp['dir'] = $linea[0];
						break;
					case 2:
						$upperspec = explode("/", $linea[0]);
						$cursp['proto'] = $upperspec[0];
						list($cursp['src'], $cursp['dst']) = explode("-", $upperspec[2]);
						$cursp['reqid'] =  substr($upperspec[3], strpos($upperspec[3], "#")+1);
						break;
				}
			}
			$i++;
		}
		if (is_array($cursp) && count($cursp))
			$spd[] = $cursp;
		pclose($fd);
	}

	return $spd;
}

/*
 * Return dump of SAD table
 */
function ipsec_dump_sad()
{
	$fd = @popen("/sbin/setkey -D", "r");
	$sad = array();
	if ($fd) {
		while (!feof($fd)) {
			$line = chop(fgets($fd));
			if (!$line || $line[0] == " ")
				continue;
			if ($line == "No SAD entries.")
				break;
			if ($line[0] != "\t")
			{
				if (is_array($cursa))
					$sad[] = $cursa;
				$cursa = array();
				list($cursa['src'],$cursa['dst']) = explode(" ", $line);
				$i = 0;
			}
			else
			{
				$linea = explode(" ", trim($line));
				switch ($i) {
					case 1:
						$cursa['proto'] = $linea[0];
						$cursa['spi'] = substr($linea[2], strpos($linea[2], "x")+1, -1);
						$reqid = substr($linea[3], strpos($linea[3], "=")+1);
						$cursa['reqid'] = substr($reqid, 0, strcspn($reqid,"("));
						break;
					case 2:
						$cursa['ealgo'] = $linea[1];
						break;
					case 3:
						$cursa['aalgo'] = $linea[1];
						break;
					case 8:
						$sadata = explode("(", $linea[1]);
						$cursa['data'] = $sadata[0] . " B";
						break;
				}
			}
			$i++;
		}
		if (is_array($cursa) && count($cursa))
			$sad[] = $cursa;
		pclose($fd);
	}

	return $sad;
}

/*
 * Return dump of mobile user list
 */
function ipsec_dump_mobile()
{
	global $g, $custom_listtags;

	$strongswan_leases_xml = '/tmp/strongswan_leases.xml';

	$_gb = exec(sprintf('/usr/local/sbin/ipsec stroke leases > %s', $strongswan_leases_xml));

	if (!file_exists($strongswan_leases_xml)) {
		log_error(gettext("IPsec daemon seems to have issues or not running! Could not display mobile user stats!"));
		return array();
	}

	$custom_listtags = array('lease', 'pool');
	$response = parse_xml_config($strongswan_leases_xml, 'leases');
	@unlink($strongswan_leases_xml);
	unset($custom_listtags, $_gb);

	return $response;
}

function ipsec_mobilekey_sort() {
	global $config;

	function mobilekeycmp($a, $b) {
		return strcmp($a['ident'][0], $b['ident'][0]);
	}

	usort($config['ipsec']['mobilekey'], "mobilekeycmp");
}

function ipsec_get_number_of_phase2($ikeid) {
	global $config;
	$a_phase2 = $config['ipsec']['phase2'];

	$nbph2=0;

	if (is_array($a_phase2) && count($a_phase2)) {
		foreach ($a_phase2 as $ph2tmp) {
			if ($ph2tmp['ikeid'] == $ikeid) {
				$nbph2++;
			}
		}
	}

	return $nbph2;
}


function ipsec_find_id(& $ph1ent, $side = "local", $rgmap = array()) {
	if ($side == "local") {
		$id_type = $ph1ent['myid_type'];
		$id_data = $ph1ent['myid_data'];

		$addr = ipsec_get_phase1_src($ph1ent);
		if (!$addr)
			return array();
	} elseif ($side == "peer") {
		$id_type = $ph1ent['peerid_type'];
		$id_data = $ph1ent['peerid_data'];

		if (isset($ph1ent['mobile']))
			$addr = "%any";
		else
			$addr = $ph1ent['remote-gateway'];
	} else {
		return array();
	}


	$thisid_type = $id_type;
	switch ($thisid_type) {
	case "myaddress":
		$thisid_type = "address";
		$thisid_data = $addr;
		break;

	case "dyn_dns":
		$thisid_type = "address";
		$thisid_data = resolve_retry($id_data);
		break;

	case "peeraddress":
		$thisid_type = "address";
		$thisid_data = $rgmap[$ph1ent['remote-gateway']];
		break;

	case "address";
		$thisid_data = $id_data;
		break;

	case "fqdn";
	case "keyid tag";
	case "user_fqdn";
	case "asn1dn";
		$thisid_data = $id_data;
		if( $thisid_data )
			$thisid_data = "{$thisid_data}";
		break;
	}
	return array($thisid_type, $thisid_data);
}


?>
