title=""
retval=""
dialog_exec=gdialog

dlg_msgbox()
{
	title=$1
	${dialog_exec} --msgbox "$title" 0 0

	${dialog_exec} --clear
	return $?
}

dlg_yesno()
{
	title=$1
	${dialog_exec} --yesno "$title" 0 0
	return $?
}

dlg_textbox()
{
	file=$1
	${dialog_exec} --textbox "$file" 0 0
	return $?
}

dlg_inputbox()
{
	title=$1
	${dialog_exec} --inputbox "$title" 0 0 "abc.txt"
	return $?
}

dlg_checklist()
{
	title=$1
	${dialog_exec} --checklist "$title" 0 0 0 \
		1 apple on \
		2 bnana off \
		3 oragne off \
		4 pear off \
			2>1.txt

	h=$(cat 1.txt)
	echo "h=$h"
	return $?
}

dlg_radiolist()
{
	title=$1
	${dialog_exec}  --radiolist "$title" 0 0 0 \
		1 apple on \
		2 bnana off \
		3 oragne off \
		4 pear off \
			2>1.txt

	h=$(cat 1.txt)
	echo "h=$h"
	return $?
}

dlg_menu()
{
	title=$1
	${dialog_exec}  --menu "$title" 0 0 0 \
		1 apple \
		2 bnana \
		3 oragne \
		4 pear \
			2>1.txt

	h=$(cat 1.txt)
	echo "h=$h"
	return $?
}

#ok=0
dlg_msgbox "helloworldxxxxxxxxxxxxxx"

#yes=0, no=1
dlg_yesno "contine"

#exit=0
dlg_textbox temp_file1

#ok=0, cancle=1
dlg_inputbox helloworld

dlg_checklist helloworld

dlg_radiolist helloworld

dlg_menu helloworld

ret=$?
echo "ret=$ret $1"

#abc=$(which ${dialog_exec})
#echo "abc=$abc"
