#!/bin/sh

module="scull"
device="scull"
mode="664"
major="260"
path="/dev"
tester="main"
logs="/var/log/kern.log"

help()
{
	echo
	echo
	echo "		This shell is used to install scull module & test it"
	echo
	echo "pdev:   print device	"
	echo "pmod:   print modules	"
	echo "plog:   print kernel logs	"
	echo "rmmod:  remove modules	"
	echo "setmod: setup modules	"
	echo "tester: call tester	"
	echo
}

pdev()
{
	
	ls -l ${path}/${module}0
	ls -l ${path}/${module}1
}

pmod()
{
	lsmod
}

plog()
{
	cat ${logs}
}

rmmod()
{

	sudo rm -f ${path}/${device}0
	sudo rm -f ${path}/${device}1
	sudo rmmod ${module}
}

setmod()
{
	sudo insmod ${module}.ko

	sudo mknod ${path}/${module}0 c ${major} 0
	sudo mknod ${path}/${module}1 c ${major} 1

	sudo chmod $mode ${path}/${module}0
	sudo chmod $mode ${path}/${module}1

	pdev
}

tester()
{
	sudo ./${tester}
}
