num=$#
param=$*
client_str=""
version_str=""
time_str=""
tgt_a=""
tgt_b=""
tgt_ota=""
file_prefix=""

out_dir="out"
shname="mkbbk"
shver="0.1"

mk_ln()
{
	ln_path="${HOME}/bin"

	if [ -e ${ln_path}/${shname} ]; then
		return 0
	else
		mkdir -p ${ln_path}
		ln -s ${PWD}/${shname}.sh ${ln_path}/${shname}
	fi
}

if [ $num -eq 0 ]; then
	echo "${shname} [client_str] [vsersion] "
	mk_ln
	exit 1
fi

if [ ${num} -ge 1 ]; then
	client_str=$1
else
	client_str=yyy
fi

if [ ${num} -ge 2 ]; then
	version_str=$2
else
	version_str=ooo
fi

get_time_string()
{
	time_str=$(date +%Y%m%d%H%M)

	echo "time: $time_str"
	return 0
}

gen_tar_file()
{
	file_name=${file_prefix}_${version_str}_${time_str}

	rm -rf ${out_dir}/${file_prefix}_${version_str}_*

	mkdir  ${out_dir}/${file_name}
	cp -rp ${out_dir}/${tgt_a}/${tgt_a}.bin $out_dir/$file_name
	cp -rp ${out_dir}/${tgt_b}/${tgt_b}.bin $out_dir/$file_name
	cp -rp ${out_dir}/${tgt_ota}/${tgt_ota}.bin $out_dir/$file_name

	echo " "
	echo "will package these files:"
	ls $out_dir/$file_name -l

	tar -cvjf ${out_dir}/${file_name}.tar.bz2 ${out_dir}/${file_name}
	echo "gen new package: ${out_dir}/${file_name} done"
	echo " "
}

mk_anc_usb_3002_bbk_rel()
{
	tgt_a=3002s_fs_boot_a
	tgt_b=3002s_fs_boot_b
	tgt_ota=3002s_ota
	file_prefix=bbk_3002s_fs_rel

	make T=$tgt_a DEBUG_PORT=1 -j4
	make T=$tgt_b DEBUG_PORT=1 -j4
	make T=$tgt_ota DEBUG_PORT=1 -j4

	get_time_string
	gen_tar_file

	echo "make 3002 for bbk done !!!"
}

mk_anc_usb_3002_ss_rel()
{
	tgt_a=ss_3002s_fs_boot_a
	tgt_b=ss_3002s_fs_boot_b
	tgt_ota=3002s_ota
	file_prefix=ss_3002s_fs_rel

	make T=$tgt_a DEBUG_PORT=1 -j4
	make T=$tgt_b DEBUG_PORT=1 -j4
	make T=$tgt_ota DEBUG_PORT=1 -j4

	get_time_string
	gen_tar_file

	echo "make 3002 for ss done !!!"
}

patch_inst()
{
	patch_file=$1
	patch_en=$2
	num=$#

	if [ $num -lt 2 ]; then
		echo "patch_inst: num=$num ,exit"
		exit 1
	fi

	if [ $patch_en -eq 1 ]; then
		patch -p1< $patch_file
		echo "patch $patch_file installed"
	else
		patch -Rp1< $patch_file
		echo "patch $patch_file uninstalled"
	fi
}

mk_anc_usb_3002_bes_demo()
{
	tgt_a=3002s_fs_boot_a
	tgt_b=3002s_fs_boot_b
	tgt_ota=3002s_ota
	file_prefix=bes_3002s_fs_demo

	make T=$tgt_a DEBUG_PORT=1 USB_ALWAYS_ON=0 HS_PLUGIN_LOW_LEVEL=1 \
		ANA_DC_CALIB_UPDATE=0 \
		USB_AUDIO_DYN_CFG=1 USB_AUDIO_24BIT=1 LOW_SIGNAL_MUTE=1 -j4

	make T=$tgt_b DEBUG_PORT=1 USB_ALWAYS_ON=0 HS_PLUGIN_LOW_LEVEL=1 \
		ANA_DC_CALIB_UPDATE=0 \
		USB_AUDIO_DYN_CFG=1 USB_AUDIO_24BIT=1 LOW_SIGNAL_MUTE=1 -j4

	make T=$tgt_ota DEBUG_PORT=1 -j4

	get_time_string
	gen_tar_file

	echo "make 3002 for demo done !!!"
}

mk_anc_usb_3002_evb()
{
	make T=3002s_dongle DEBUG_PORT=1 HS_PLUGIN_LOW_LEVEL=0 -j4
	echo "make 3002s_evb done !!!"
}

mk_anc_usb_3002_dongle()
{
	make T=3002s_dongle DEBUG_PORT=1 HS_PLUGIN_LOW_LEVEL=1 -j4
	echo "make 3002s_dongle done !!!"
}

mk_anc_usb_3002s_fs()
{
	make T=3002s_fs DEBUG_PORT=1 -j4
	echo "make 3002s_fs done !!!"
}

burn_tgt()
{
	abs_dld_path=$HOME/workspace/bes/bin-flasher
	tgt=$1
	dev_id=${version_str}

	if [ "${dev_id}" == "ooo" ]; then
		dev_id=0
	fi
	dev_port=/dev/ttyUSB${dev_id}

	#ls -l $abs_dld_path

	echo "$abs_dld_path/dldtool ${dev_port} ${abs_dld_path}/programmer3002.bin out/${tgt}/${tgt}.bin"

	$abs_dld_path/dldtool ${dev_port} ${abs_dld_path}/programmer3002.bin out/${tgt}/${tgt}.bin

	echo "burning ${tgt}.bin done !!! ..."
}

clean_tgt()
{
	tgt=$1

	echo "rm -rf ${out_dir}/${tgt}_*"
	rm -rf ${out_dir}/${tgt}_*
}

mk_tgt()
{
	tmp=$1
	ver=$2
#	echo "$tmp, $ver"

	case $tmp in
	-bbk )
		mk_anc_usb_3002_bbk_rel
		;;
	-ss | --samsung )
		mk_anc_usb_3002_ss_rel
		;;
	-bes )
		patch_inst 3002_bes_demo.diff 1
		mk_anc_usb_3002_bes_demo
		patch_inst 3002_bes_demo.diff 0
		;;
	-evb )
		mk_anc_usb_3002_evb
		;;
	-dongle )
		mk_anc_usb_3002_dongle
		;;
	-fs )
		mk_anc_usb_3002s_fs
		;;
	-burn_dongle | -burn_evb )
		burn_tgt 3002s_dongle
		;;
	-burn_fs )
		burn_tgt 3002s_fs
		;;
	-h | --help )
		show_help
		;;
	-c | --clean )
		clean_tgt bes_3002s_fs_demo
		clean_tgt bbk_3002s_fs_rel
		;;
	-t | --test )
		echo "hello world, this is ${shname}.sh"
		;;
	-v | --version )
		echo "${shname} version $shver"
		;;
	* )
		echo "unknown command: $tmp"
		show_help
		return 1;
		;;
	esac

	return 0
}

show_help()
{
	echo "usage:"

	echo "  ${shname} -bbk [rel_id]"
	echo "  ${shname} -ss  [rel_id]"
	echo "  ${shname} -bes [rel_id]"
	echo "  ${shname} -evb         "
	echo "  ${shname} -dongle      "
	echo "  ${shname} -fs          "
	echo "  ${shname} -burn_fs [port_id]"
	echo "  ${shname} -burn_evb [port_id]"
	echo "  ${shname} -burn_dongle [port_id]"
	echo "  ${shname} -h or --help"
	echo "  ${shname} -c or --clean"
	echo "  ${shname} -t or --test"
	echo "  ${shname} -v or --version"
}

mk_tgt $client_str $version_str

#exit 0
