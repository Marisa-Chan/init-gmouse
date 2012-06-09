#!/bin/sh

PROG="a4_tool"
disp="0"
progdir=".a4_tool"
base_path="/usr"

mouse=`$base_path"/bin/"$PROG bat | grep -i mouse`
kbd=`$base_path"/bin/"$PROG bat | grep -i keyboard`

export DISPLAY=$disp


if [ ! -d ~/"$progdir" ]
then
mkdir ~/"$progdir"
fi


if [ "$mouse" != "No mouses found." ]
then
mouse=`echo -n $mouse | awk '{print $3}' | tr -d [%]`
m_p=""

	if [ -f ~/"$progdir/mouse_val" ]
	then
	m_p=`cat ~/"$progdir/mouse_val"`
	fi
	
	if [ "$mouse" != "$m_p" ]
	then
		if [ "$mouse" -le "50" ]
		then
		notify-send -i $base_path/share/a4_tool/mouse.png "Mouse battery low: $mouse%"
		fi
	fi
	
	echo $mouse > ~/"$progdir/mouse_val"
fi

if [ "$kbd" != "No keyboards found." ]
then
kbd=`echo -n $kbd | awk '{print $3}' | tr -d [%]`
m_p=""

	if [ -f ~/"$progdir/kbd_val" ]
	then
	m_p=`cat ~/"$progdir/kbd_val"`
	fi
	
	if [ "$kbd" != "$m_p" ]
	then
		if [ "$kbd" -le "50" ]
		then
		notify-send -i $base_path/share/a4_tool/keyboard.png "Keyboard battery low: $kbd%"
		fi
	fi
	
	echo $kbd > ~/"$progdir/kbd_val"
fi