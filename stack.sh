#!/bin/bash

num_arr=()
num_pos=100
op_arr=()
op_pos=100
push()
{
	if [ `eval echo \\$$2` -eq 0 ];then
		return
	fi
	let "$2-=1"
	eval "$1[\$$2]=\"$3\""
}
pop()
{
	if [ `eval echo \\$$2` -eq 100 ];then
		return
	fi
	eval "$3=\"\${$1[\$$2]}\""
	#unset $1[`eval echo \\$$2`]
	eval "unset $1[\$$2]"
	let "$2+=1"
	
}
get_top()
{
	if [ `eval echo \\$$2` -eq 100 ];then
		return
	fi
	eval "$3=\"\${$1[\$$2]}\""
	
}

op_priority()
{
	priorities='=1+2-2*3/3)4(5'
	echo "$1"
	local index=
	if [ "$1" == "+" ];then

		index=`expr index $priorities "\+"`
	else

		index=`expr index $priorities "$1"`
	fi
	echo "index=$index"
	if [ $index  -eq 0 ];then
		return 0
	fi
	local next=`expr $index + 1`
	echo $next
	return `expr substr $priorities $next 1`
	
}
calc()
{
	pro=$1
	op=
	get_top op_arr op_pos op
	if [ "$op" == "*" ];then

	op_priority  "*"
	pro1=$?
    else

	op_priority  $op
	pro1=$?
	fi
	echo "pro1=$pro1"
	num1=
	num2=
	res=
	while [ $pro1 -gt 0 ] &&  [ $pro1 -ge $pro ] ;do
		pop num_arr num_pos num1
		pop num_arr num_pos num2
		pop op_arr op_pos op
		echo "num1=${num1},num=${num2}"
		if [ "$op" == "*" ];then

		res=`expr $num1 \* $num2`
		else

		res=`expr $num1 $op $num2`
		fi
		echo ${num_arr[@]}
		echo ${op_arr[@]}
		push num_arr num_pos $res
		op=
		get_top op_arr op_pos op
		op_priority  "$op"
		pro1=$?
	done
}
#op_priority '\\*'
#echo $?
#exit
e="20+4*5-3+6/9-2-10"
len=${#e}
i=0
num=
while [ $i -lt $len ];do
	eval "op=\${e:$i:1}"
	case $op in
		'+')
			push num_arr num_pos $num
			num=
			op_priority "$op"
			op_prio=$?
			top_op= 
			get_top op_arr op_pos top_op
			if [ -n "$top_op" ];then
				echo "top_op=$top_op"	
			op_priority "$top_op" 
			top_op_prio=$?
			if [ $op_prio  -lt $top_op_prio ];then
				calc $op_prio
			fi
			fi
			push op_arr op_pos "$op"
			;;
		'-')
			push num_arr num_pos $num
			op_priority $op 
			op_prio=$?
			top_op= 
			get_top op_arr op_pos top_op
			op_priority "$top_op" 
			top_op_prio=$?
			if [ $op_prio  -lt $top_op_prio ];then
				calc $op_prio
			fi
			echo "-------${op_arr[@]}"
			if [ -z "$num" ];then
				push op_arr op_pos '('	
				push num_arr num_pos 0
				push op_arr op_pos $op
				push num_arr num_pos $num
				push op_arr op_pos ')'
			else
				
			push op_arr op_pos "$op"
			echo "-------${op_arr[@]}"
			fi
			num=
			;;
		
		'*')
			push num_arr num_pos $num
			num=
			op_priority "$op" 
			op_prio=$?
			top_op= 
			get_top op_arr op_pos top_op
			op_priority "$top_op" 
			top_op_prio=$?
			#echo "op_prio=$op_prio"
			if [ $op_prio  -lt $top_op_prio ];then
				calc $op_prio
			fi
			push op_arr op_pos "$op"
			echo "---${op_arr[@]}"
			;;
		'/')
			push num_arr num_pos $num
			num=
			op_priority "$op" 
			op_prio=$?
			top_op= 
			get_top op_arr op_pos top_op
			op_priority "$top_op" 
			top_op_prio=$?
			if [ $op_prio  -lt $top_op_prio ];then
				calc $op_prio
			fi
			push op_arr op_pos "$op"
			;;

		'(')
			push num_arr num_pos $num
			num=
			push op_arr op_pos "$op"
			;;
		')')
			op_priority "$op" 
			op_prio=$?
			top_op = 
			get_top op_arr op_pos top_op
			op_priority "$top_op" 
			top_op_prio=$?
			push num_arr num_pos $num
			if [ $op_prio  -lt $top_op_prio ];then
				calc $op_prio
			fi
			num=
			push op_arr op_pos "$op"
			;;
		[0-9]*)
			num="$num$op"
			;;
	esac
	echo "$i:"
	let "i+=1"
done
if [ -n "$num" ];then
	
	push num_arr num_pos $num
	num=
fi
echo ${num_arr[@]}
echo ${op_arr[@]}
