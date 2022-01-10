stty -tabs cr0 ff0 nl0 erase '^H' kill '^X'
TERM=vt100
export TERM
PATH=:/bin:/usr/bin:/usr/window/bin
setupinv
exec /usr/window/wsh
