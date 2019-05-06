num=$#
param=$*
out_dir=out
client_str=""
version_str=""
time_str=""
tgt_a=""
tgt_b=""
tgt_ota=""
file_prefix=""

if [ $num -eq 0 ]
then
	echo "./mk_bbk.sh [client_str] [vsersion] "
	exit 1
fi

if [ ${num} -ge 1 ]
then
	client_str=$1
else
	client_str=yyy
fi

if [ ${num} -ge 2 ]
then
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

	rm -rf ${out_dir}/${file_prefix}_*

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

mk_anc_usb_3002_evb()
{
	make T=3002s_dongle DEBUG_PORT=1 -j4
	echo "make 3002s_dongle done !!!"
} 

mk_anc_usb_3002s_fs()
{
	make T=3002s_fs DEBUG_PORT=1 -j4
	echo "make 3002s_fs done !!!"
}

mk_tgt()
{
	tmp=$1
	ver=$2
	echo "$tmp, $ver"

	case $tmp in
	bbk | BBK )
		echo "bbk"
		mk_anc_usb_3002_bbk_rel
		;;
	ss | SS | samsung | SAMSUNG )
		echo "samsung"
		mk_anc_usb_3002_ss_rel
		;;
	dongle | evb )
		echo "3002s_dongle"
		mk_anc_usb_3002_evb
		;;
	fs )
		echo "3002s_fs"
		mk_anc_usb_3002s_fs
		;;
	* ) echo "unknown target: $tmp"
		return 1;
		;;
	esac

	return 0;
}

mk_tgt $client_str $version_str

exit 0
