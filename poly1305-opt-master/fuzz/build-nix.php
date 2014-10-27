<?php
	$debug_echo = false;

	function echoln($str) {
		echo $str;
		echo "\n";
	}

	function usage($reason) {
		echoln("Usage: php build-nix.php [flags]");
		echoln("Flags in parantheses are optional");
		echoln("");
		echoln("  --bits=[32,64]");
		echoln("  --ref=[8,32]                        which reference version to use, 8 bit or 32 bit");
		echoln("  --versions={...}                    comma separated list of which versions to build");
		echoln("    * x86");
		echoln("    * sse2");
		echoln("    * avx");
		echoln("    * avx2");
		echoln(" (--compiler=[*gcc,clang,icc])        which compiler to use, gcc is default");
		echoln(" (--arch=[*x86])                      which arch to use, x86 is default");
		echoln(" (--out=filename)");
		echoln(" (--debug)                            echos build commands and compiles with -g");
		echoln("");
		if ($reason)
			echoln($reason);
	}

	function cleanup() {
		system("rm -f *.o");
	}

	function trycmd($desc, $cmd) {
		global $debug_echo;
		echoln($desc);
		if ($debug_echo)
			echoln($cmd);

		$ret = 0;
		system($cmd, $ret);
		return $ret == 0;
	}


	function runcmd($desc, $cmd) {
		if (!trycmd($desc, $cmd)) {
			cleanup();
			exit;
		}
	}

	class argument {
		var $set, $value;
	}

	class anyargument extends argument {
		function anyargument($flag) {
			global $argc, $argv;

			$this->set = false;

			for ($i = 1; $i < $argc; $i++) {
				if (!preg_match("!--".$flag."=(.*)!", $argv[$i], $m))
					continue;
				$this->value = $m[1];
				$this->set = true;
				return;
			}
		}
	}

	class multiargument extends anyargument {
		function multiargument($flag, $legal_values) {
			parent::anyargument($flag);

			if (!$this->set)
				return;

			$map = array();
			foreach($legal_values as $value)
				$map[$value] = true;

			if (!isset($map[$this->value])) {
				usage("{$this->value} is not a valid parameter to --{$flag}!");
				exit(1);
			}
		}
	}

	class listargument extends anyargument {
		function listargument($flag, $legal_values) {
			parent::anyargument($flag);

			if (!$this->set)
				return;

			$map = array();
			foreach($legal_values as $value)
				$map[$value] = true;

			$this->value = explode(",", $this->value);
			foreach($this->value as $value) {
				if (!isset($map[$value])) {
					usage("{$value} is not a valid parameter to --{$flag}!");
					exit(1);
				}
			}
		}
	}


	class flag extends argument {
		function flag($flag) {
			global $argc, $argv;

			$this->set = false;

			$flag = "--{$flag}";
			for ($i = 1; $i < $argc; $i++) {
				if ($argv[$i] !== $flag)
					continue;
				$this->value = true;
				$this->set = true;
				return;
			}
		}
	}

	$bits = new multiargument("bits", array("32", "64"));
	$ref = new multiargument("ref", array("8", "32"));
	$compiler = new multiargument("compiler", array("gcc", "clang", "icc"));
	$versions = new listargument("versions", array("x86", "sse2", "avx", "avx2"));
	$debug = new flag("debug");
	$out = new anyargument("out");

	$debug_echo = $debug->set;

	$err = "";
	if (!$bits->set)
		$err .= "--bits not set\n";

	if (!$ref->set)
		$err .= "--ref not set\n";

	if (!$versions->set)
		$err .= "--verions not set\n";

	if ($err !== "") {
		usage($err);
		exit;
	}

	$compile = ($compiler->set) ? $compiler->value : "gcc";
	$link = "";
	$flags = ($debug->set) ? "-g -m{$bits->value}" : "-O3 -m{$bits->value}";
	$filename = ($out->set) ? $out->value : "fuzz-poly1305-{$bits->value}";

	runcmd("building ref..", "{$compile} {$flags} ../extensions/poly1305_ref-{$ref->value}.c -c -o poly1305_ref.o");

	foreach($versions->value as $version) {
		$name = "poly1305_{$version}";
		$file = "{$name}-{$bits->value}";
		runcmd("building {$version}..", "{$compile} {$flags} ../extensions/{$file}.S -c -o {$file}.o");
		$link .= "{$file}.o -D".strtoupper($name)." ";
	}
	runcmd("linking..", "{$compile} {$flags} {$link} fuzz-poly1305.c poly1305_ref.o -o {$filename}");
	echoln("{$filename} built.");


	cleanup();
?>
