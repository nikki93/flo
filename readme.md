# flo

flo is a command line application that keeps you in check. It’s small and fast.

## usage

    $ flo -w "clean apartment"
    $ flo -w "eat lunch" -a "some cafe" -f 181430 -t 181530
    $ flo -w "a deadline" -a "uni" -t 10011200

    $ flo
       0  2010-07-18 14:30  eat lunch@some cafe
          2010-07-18 15:30
       1  2010-10-01 12:00d a deadline
       2  clean apartment

Add `alias f='flo'` to your `~/.bashrc`.

## license

ISC.
