#!/bin/bash
# 四则运算

# 全局变量
num_arr=()  #保存 操作数
num_pos=100 # 当前 保存操作数 的数组位置
op_arr=() #保存 操作符
op_pos=100 # 当前 保存 操作符的 数组 位置
# + * / - 前 没有操作数时，若 为) 后，则 该标示设置为 2，
#若 不是 在) 后且 为 - 则设置为0，表示一元操作符
sub_first_num=1 

push()
{
	if [ `eval echo \\$$2` -eq 0 ];then
		return
	fi
	let "$2-=1"
	# $3 表示保存的操作符，当为* 时，在eval 中会进行扩展展开，故需添加""
	eval "$1[\$$2]=\"$3\"" 
}
pop()
{
	if [ `eval echo \\$$2` -eq 100 ];then
		return
	fi
	# 保存 栈的第一个元素，若为* 时，在 eval 中会进行扩展展开，故需添加""
	eval "$3=\"\${$1[\$$2]}\""
	# 以下两种形式都可以
	#unset $1[`eval echo \\$$2`]
	eval "unset $1[\$$2]" 
	let "$2+=1"
	
}
get_top()
{
	if [ `eval echo \\$$2` -eq 100 ];then
		return
	fi
	# 只获取 栈的头部数据，不进行弹出，因此 不修改$2 值
	eval "$3=\"\${$1[\$$2]}\""
	
}

# 获取 操作数 的优先级
op_priority()
{
	#优先级列表 ，其中() 优先级 为最小
	priorities='(0)0=1+2-2*3/3'
	local index=
	# 在 expr 中 对于 + ( )符号 需进行转义
	if [ "$1" == "+" ] || [ "$1" == "(" ] || [ "$1" == ")" ] ;then

		index=`expr index $priorities "\\\\$1"`
	else

		index=`expr index $priorities "$1"`
	fi
	if [ $index  -eq 0 ];then
		return 0
	fi
	local next=`expr $index + 1`
	return `expr substr $priorities $next 1`
	
}
# 计算 不小于 当前操作符 优先级 的 之前的操作符
# 对于 - / 操作数 不能交换，因此 必须 包括优先级相等的操作符
calc()
{
	local prio=
	local op="$1" # 接受 当前的操作符，对于*, 需使用"",否则进行展开
	local top_op=
	# 当 参数为空时，则执行全部计算
	if [ -z "$op" ];then
	prio=0
	else
	
	op_priority "$op" # 调用函数时，传入的参数若为 *, 需使用"",否则进行展开
	prio=$?
	fi
	# 获取 栈中的操作符，但不弹出
	get_top op_arr op_pos top_op
	op_priority "$top_op" # 调用函数时，传入的参数若为 *, 需使用"",否则进行展开
	top_prio=$?
	local num1=
	local num2=
	local res=
	# 遇到 ( 时，退出循环
	while [ $top_prio -gt 0 ] &&  [ $top_prio -ge $prio ] ;do
		#先弹出的为 第二个操作数， 后弹出的为 第一个操作数
		pop num_arr num_pos num2
		pop num_arr num_pos num1
		# 将 操作符 弹出
		pop op_arr op_pos top_op
		#echo "num1=${num1},num2=${num2}"
		# expr 对于* 需 使用\ 转义
		if [ "$top_op" == "*" ];then

		res=`expr $num1 \* $num2`
		else

		res=`expr $num1 $top_op $num2`
		fi
		# 将计算的结果 重新保存到 数字数组中
		push num_arr num_pos $res
		#echo ${num_arr[@]}
		#echo "${op_arr[@]}"
		top_op=
		# 获取 栈中的操作符，但不弹出
		get_top op_arr op_pos top_op
		op_priority  "$top_op"  # 调用函数时，传入的参数若为 *, 需使用"",否则进行展开
		top_prio=$?
	done
}
# 处理 解析到的操作符
exec_op()
{
	local op="$1" # 接受 当前操作符
	local num=$2 # 接受 当前操作符 前的操作数
	# 当 当前操作符 的第一操作数 不为空时
	if [ -n "$num" ];then
			# 当 当前操作符 的前一个操作符 为 -，即$num 为 - 的第二个操作数 
			# 且该 - 的第一操作数 为 空时(在之前处理 - 时 不会将 - 压入栈中 )，
			#则 保存  - 的第二个操作数的相反数 
	       if [ $sub_first_num == 0 ];then
			num=`expr 0 - $num`
			sub_first_num=1 # 更新 标志为原始状态
		fi
		push num_arr num_pos $num # 先 将操作数 压入栈后，执行该操作数前的操作
		calc "$op"
	  	push op_arr op_pos "$op" # 将 当前操作符 压入栈
	# 当 当前操作符 位于 ) 后时 ，则 执行 之前的操作，并将 当前操作符压入栈
	# 对于 不是位于), 而 第一操作数为空时，则不执行任何操作
	elif [ $sub_first_num -eq 2 ] ;then
			
		calc "$op"
	  	push op_arr op_pos "$op"
	fi
	
	
}

#op_priority '\\*'
#echo $?
#exit
e="20+4*(-5)-3+-10-18/(9-3)-10"
len=${#e}
i=0
num=
# 解析 数值字符串
while [ $i -lt $len ];do
	eval "op=\${e:$i:1}"
	case $op in
		'+')
			exec_op "$op" $num
			num=
			echo "+++#${op_arr[@]}"
			;;
		'-')
			exec_op "$op" $num
			# 当第一操作数为空 且  不是位于) 后时，修改 sub_first_num 标示
			if [ $sub_first_num -eq 1 ]  &&  [ -z "$num" ];then
				sub_first_num=0
			else
			 	num=	
			fi
			echo "---#${op_arr[@]}"
			;;
		
		'*')
			exec_op "$op" $num
			num=
			echo "***#${op_arr[@]}"
			;;
		'/')
			exec_op "$op" $num
			num=
			echo "/#${op_arr[@]}"
			;;

		'(')
			if [ -n "$num" ];then

				push num_arr num_pos $num
				num=
			fi
			# 只将( 压入栈，用于 )匹配
			push op_arr op_pos "$op"
			echo "(#${op_arr[@]}"
			;;
		')')
			if [ $sub_first_num -eq 0 ];then
				num=`expr 0 - $num`
			fi
			#将) 前的操作数 压入栈 后执行之前的操作
			#) 不压入 栈
			push num_arr num_pos $num
			calc "$op"
			# 设置 ) 标示
			sub_first_num=2
			top_op= 
			get_top op_arr op_pos top_op
			op_priority "$top_op" 
			top_op_prio=$?
			# 将栈中的 ( 弹出 
			if [  $top_op_prio -eq 0 ];then
				pop op_arr op_pos top_op
			fi
			num=
			;;
		[0-9]*)
			num="$num$op" # 拼接多位数 操作数
			;;
	esac
	echo "$i:"
	let "i+=1"
done
# 保存  最后一个的操作数
if [ -n "$num" ];then
	
	push num_arr num_pos $num
	num=
fi
#echo ${num_arr[@]}
#echo ${op_arr[@]}
# 执行 全部 运算
calc 
echo ${num_arr[@]}
