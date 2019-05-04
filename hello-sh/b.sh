echo "param number: $#"
echo "param list  : $*"
echo "param list  : $@"

################### case 1 ########################
num=$#
echo "num=${num}";

if [ ${num} -ge 1 ];
then
	echo "param[1]: $1"
fi

if [ ${num} -ge 2 ];
then
	echo "param[2]: $2"
fi

if [ ${num} -ge 3 ];
then
	echo "param[3]: $3"
fi

################### case 2 ########################
list=$*
j=0;
for i in ${list};
do
	echo "list[$j]=$i";
	j=$((j+1));
done

################### case 3 ########################
i=0;
n=0;
while true;
do
	if [ ${n} -eq ${i} ]
	then
		echo "no param, exit";
		break;
	fi;

	i=$((i+1));
	echo "tick ${i}"

	sleep 1
done


################### case 4 ########################
set list_user
set list_password
list_user="abc \
	   def \
	   jkl \
	   ggg \
	   hhh"
list_password="111 222 333"

set user_password;
set user_name;
set user_tmp;
set retval;

read_cmd()
{
	echo "input your $1"
	read user_tmp;
	echo "your input: $user_tmp"
}

check_cmd_str()
{
	list=$*
	j=$user_tmp

	echo "list=$list"
	echo "j=$j"

	for i in $list;
	do
		if [ "$i" == "$j" ];
		then
			echo "check ok"
			return 0;
		fi
	done;
	return 1;
}

check_cmd_dig()
{
	list=$*
	j=$user_tmp

	echo "list=$list"
	echo "j=$j"

	for i in $list;
	do
		if [ $i -eq $j ];
		then
			echo "check ok"
			return 0;
		fi
	done;
	return 1;
}

read_user_name()
{
	read_cmd name
	check_cmd_str ${list_user}

	retval=$?
	if [ $retval -eq 0 ];
	then
		user_name=user_tmp;
		return 0;
	fi

	echo "user invalid"
	return 1;
}

read_user_password()
{
	read_cmd password
	check_cmd_dig ${list_password}

	retval=$?
	if [ $retval -eq 0 ];
	then
		user_password=user_tmp;
		return 0;
	fi

	echo "password invalid"
	return 1;
}

login_in()
{
	while true;
	do
		echo " "
		echo " "
		echo "*****************************"

		read_user_name
		read_user_password

		echo "user_name=${user_name}"
		echo "user_password=${user_password}"

		echo "continue ? y/n (default=y)"
		read retval

		case "$retval" in
		y | yes | YES | Yes ) echo "again";;
		n | no | NO | No )
			echo "exit";
			echo "exit2";
			echo "exit3";
			exit 0;;
		* ) echo "default is yes";;
		esac
	done;
}

login_in

exit 0
