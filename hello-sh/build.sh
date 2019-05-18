ans=""

#############################################
# combox list: select only one item
# ans: item name
# $? : item idx
#############################################

combox_list()
{
	ans=""

	title=$1
	if [ "$title" != "" ]; then
		echo "$title"
	else
		echo "items:"
	fi

	while [ $1 != "" ]; do
		shift
		break;
	done

	j=0
	items=$*

	if [ -z $1 ]; then
		echo "null item"
		exit -1;
	fi

	for i in ${items}; do
		j=$((j+1))
		echo "	[$j]	$i"
	done

	set k
	read k
	if [ $k -gt $j ]; then
		echo "invalid input $k"
		exit -2;
	fi

	j=0
	for i in ${items}; do
		j=$((j+1))
		if [ $j -eq $k ]; then
			ans=$i
			break
		fi
	done

	return $k
}

#############################################
# check_box: select multiple items
# ans: item name
# $? : item idx
#############################################

check_box()
{
	ans=""

	title=$1
	if [ "$title" != "" ]; then
		echo "$title"
	else
		echo "items:"
	fi

	while [ $1 != "" ]; do
		shift
		break;
	done

	if [ -z $1 ]; then
		echo "null item"
		exit -1;
	fi

	j=0
	items=$*
	for i in ${items}; do
		j=$((j+1))
		echo "	[$j] 	$i"
	done

	set k
	read k

	for i in $k; do
		if [ $i -gt $j ]; then
			echo "invalid input $k"
			exit -2;
		fi
	done

	num=0
	for n in $k; do
		j=0
		for i in ${items}; do
			j=$((j+1))

			if [ $j -eq $n ]; then
				ans+="$i "
				num=$((num+1))
			fi
		done
	done

#	echo "num=$num"
	return $num
}

#############################################
# question_yesno: question yes or no
# ans: yes or no
# $? : 0
#############################################

question_yesno()
{
	ans=""

	title=$1
	if [ -z $title ]; then
		title="continue"
	fi

	echo "$title(yes/no) ? ,default is yes)"

	read k
	case $k in
	y | yes | YES | Y )
		echo "yes";;
	n | no  | NO  | N )
		echo "no";
		exit 0;;
	* )
		echo "yes";;
	esac
	ans="yes"
	return 0;
}

#############################################
# list_dir: list directory name
# ans: directory name list
# $? : number of list
#############################################

list_dir()
{
	ans=""
	dir=$1

	if [ -d $dir ]; then
		echo "$dir:"
	else
		echo "not exist $dir"
		exit -1
	fi

	if [ $dir == "" ]; then
		echo "null dir"
		exit -1
	fi
	ans=$(ls $dir)
	j=0
	for i in $ans; do
		j=$((j+1))
	done
	echo "dir num:$j"

	return $j
}

#############################################
#############################################

chips="2300 1400 3001 3002"
debug="uart0 uart1"

build_tgt()
{
	list_dir "config/"
	dirs=$ans

	check_box "targets:" $dirs
	tgt=$ans

	combox_list "chips:" $chips
	chip=$ans

	combox_list "debug:" $debug
	port=$?

	echo "T=$tgt"
	echo "CHIP=$chip"
	echo "DEBUG_PORT=$port"

	question_yesno

	for i in $tgt; do
		make T=$i CHIP=best${chip} DEBUG_PORT=${port} -j4
	done
}

build_tgt
