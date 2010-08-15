# flo

A command line app for organizing events, to-dos, and deadlines.

## Installation

    make
    cp flo /usr/local/bin/

## Usage

### List items

    flo

### List items of a tag

    flo .tag

### Add item

    flo [.tag] what[,from][-to]

    flo [-T tag] -w what [-f from | -t to]

### Change item

For fields other than `-w what`, value set to `r` removes the field.

    flo -c id -T tag | -w what | -f from | -t to

### Remove item

    flo -r id

### Date formats

    YYMMDDhhmm
      MMDDhhmm
        DDhhmm
        DDhh
        DD

If the year or the month isn’t specified, the current year and month is used.
For formats without a month, if the date specified is before today’s date, the
month is set to the next month.

`td`, `tm`, or `+n` can be used in place of `DD`, where `td` is today, `tm` is
tomorrow, and `+n` sets the date `n` days from today.

The value for hours and minutes is set to `00` if no other value is specified.

## Introduction

### Add an event

To add an event, specify what to do, the from-, and the to-date. Today’s date
is `2010-08-05` in these examples.

    $ flo eat lunch,051100-051130
       0       today 11:00  eat lunch
                     11:30

Alternatively, use `td` in place of today’s date. It’s not required to specify
a to-date when adding an event.

    $ flo watch movie,td19
       0       today 11:00  eat lunch
                     11:30
       1       today 19:00  watch movie

### Add a to-do

A to-do does not have any dates. flo lists to-dos with `t` at the start of the
line. This is useful when grepping flo’s output.

    $ flo clean apartment
       0       today 11:00  eat lunch
                     11:30
       1       today 19:00  watch movie
    t  2  clean apartment

### Add a deadline

A deadline only has a to-date. When using a short format and the date is less
than the date of today, flo assumes that it belongs to the next month.

    $ flo deliver assignment-04
       0       today 11:00  eat lunch
                     11:30
       1       today 19:00  watch movie
    d  2       09-04 00:00  deliver assignment
    t  3  clean apartment

### Add a tagged event

A tag starts with ‘.’ and does not contain spaces.  I recommend using short tags
such as ‘w’ for work, ‘u’ for university, ‘b’ for things to buy etc.

    $ flo .w meeting,tm14
       0       today 11:00  eat lunch
                     11:30
       1       today 19:00  watch movie
       2    tomorrow 14:00  .w meeting
    d  2       09-04 00:00  deliver assignment
    t  4  clean apartment

### Look up tagged items

The tag is not printed in the listing.

    $ flo .w
       2    tomorrow 15:00  meeting

### Change an item

It’s possible to change the `T` – tag, `w` – what, `f` – from-date, and `t` –
to-date of an item. For all fields except `w`, the value  `r` removes the
field.

`2` is the current id of the “meeting” event. The id is used when changing or
removing items.

    $ flo -c 2 -f tm15
       0       today 11:00  eat lunch
                     11:30
       1       today 19:00  watch movie
       2    tomorrow 15:00  .w meeting
    d  2       09-04 00:00  deliver assignment
    t  4  clean apartment

### Remove an item

After eating your lunch, you might want to remove the event.

    $ flo -r 0
       0       today 19:00  watch movie
       1    tomorrow 15:00  .w meeting
    d  2       09-04 00:00  deliver assignment
    t  3  clean apartment

## Aliases

    alias f='flo'

    # events
    alias fe='flo | grep ^[^td]' 

    # to-dos
    alias ft='flo | grep ^t' 

    # deadlines
    alias fd='flo | grep ^d' 

    # today’s events and deadlines
    alias ftd='flo | grep today' 

    # tomorrow’s events and deadlines
    alias ftm='flo | grep tomorrow' 

## Items file

Items are written to `$HOME/.flo`. Each item is stored on a separate line, and
each field is separated by a tab.

    what\tfrom\tto\ttag

## License

Copyright © 2010, Alexander Teinum <ateinum@gmail.com>

Permission to use, copy, modify, and/or distribute this software for any purpose
with or without fee is hereby granted, provided that the above copyright notice
and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
THIS SOFTWARE.
