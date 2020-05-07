proc bind_first { f x } {
    set c "proc helper$f$x { y } { return \[ $f $x \$y \]; } ; return helper$f$x;"

    # For debuggin
    #puts $c

    eval $c
}


proc sum { a b } {
    return [ expr $a + $b ]
}


set plus3 [ bind_first sum 3 ]


puts [ $plus3 4 ]

