set user_list

user_list+=" jack"
user_list+=" bob"
user_list+=" kate"
user_list+=" alice"

for i in $user_list;
do
	echo "user_list: $i"
done

set flag user name

while true;
do
	user="none";

	echo "input your name"
	read name
	echo "name: $name"

	for i in $user_list;
	do
		if [ "$i" == "$name" ];
		then
			user=$i;
			break;
		fi
	done

	if [ $user != "none" ];
	then
		flag=1;
		break;
	else
		echo "invalid user"
		flag=0;
	fi
done

echo "user=$user, flag=$flag"

exit
