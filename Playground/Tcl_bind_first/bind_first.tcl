proc bind_first { f x } {
    set name "bind_result<${f},${x}>"
    set c "proc $name { y } { return \[ $f $x \$y \]; }"

    # For debuggin
    puts $c

    eval $c
    return $name
}


proc sum { a b } {
    return [ expr $a + $b ]
}


set plus3 [ bind_first sum 3 ]


puts [ $plus3 4 ]

