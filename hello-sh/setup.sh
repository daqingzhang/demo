
	id=$$
	param=$@
	num=$#
	argv1=$1
	argv2=$2
	tmp_file=temp_file1

	echo "id: $id"
	echo "param: $param"
	echo "param num: $num"
	echo "shell: $0"
	echo "first is $argv1"
	echo "second is $argv2"

	touch ${tmp_file}

	echo "input:"
	read DAT

	echo $id >> ${tmp_file}
	echo $param >> ${tmp_file}
	echo $num >> ${tmp_file}
	echo ${DAT} >> ${tmp_file}

	echo "read out"
	cat ${tmp_file}
	echo ""


func1()
{
	echo "first=$1"
	echo "second=$2"
}

func1 a b
