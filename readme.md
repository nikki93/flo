# flo

flo is a command line application that keeps you in check. It’s small and fast.

## Installation

    make
    cp flo /usr/local/bin/

Optionally set up `f` as an alias to `flo`.

    echo "alias f='flo'" >> ~/.bashrc

## Getting started

Today’s date is `2010-08-05` in these examples.

### Add an event

To add an event, specify what to do, the from- and the to-date.

    $ f eat lunch,051100-051130
       0       today 11:00  eat lunch
                     11:30

Alternatively, use `td` in place of today’s date. You aren’t required to specify
a to-date when adding an event.

    $ f watch movie,td19
       0       today 11:00  eat lunch
                     11:30
       1       today 19:00  watch movie

### Add a to-do

A to-do is like an event, but without any dates. A to-do is prefixed with a `t`.
This can be useful when grepping flo’s output.

    $ f clean apartment
       0       today 11:00  eat lunch
                     11:30
       1       today 19:00  watch movie
    t  2  clean apartment

### Add a deadline

An event with only a to-date is a deadline. When using a short format and the
date is less than the date of today, flo assumes that it belongs to the next
month.

    $ f deliver assignment-04
       0       today 11:00  eat lunch
                     11:30
       1       today 19:00  watch movie
    d  2  2010-09-04 00:00  deliver assignment
    t  3  clean apartment

### Tag an item

An item might be tagged. A tag starts with ‘.’ and does not contain spaces.

    $ f .work meeting,tm14
       0       today 11:00  eat lunch
		     11:30
       1       today 19:00  watch movie
       2    tomorrow 14:00  .work meeting
    d  2  2010-09-04 00:00  deliver assignment
    t  4  clean apartment

### Change an item

It’s possible to change the `T` – tag, `w` – what, `f` – from-date, and `t` –
to-date of an item. For all fields except `w`, the value  `r` removes the
field.

`2` is the current id of the “meeting” event. The id is used when changing or
removing items.

    $ f -c 2 -f tm15
       0       today 11:00  eat lunch
		     11:30
       1       today 19:00  watch movie
       2    tomorrow 15:00  .work meeting
    d  2  2010-09-04 00:00  deliver assignment
    t  4  clean apartment

I recommend using short tags such as ‘w’ for work, ‘u’ for university,
‘b’ for things to buy etc.

    $ f -c 2 -T w
       0       today 11:00  eat lunch
		     11:30
       1       today 19:00  watch movie
       2    tomorrow 15:00  .w meeting
    d  2  2010-09-04 00:00  deliver assignment
    t  4  clean apartment

### Look up tagged items

Looking up items belonging to a tag then becomes fast. The tag is not printed
in the listing.

    $ f .w
       2    tomorrow 15:00  meeting

### Remove an item

After eating your lunch, you might want to remove the event.

    $ f -r 0
       0       today 19:00  watch movie
       1    tomorrow 15:00  .w meeting
    d  2  2010-09-04 00:00  deliver assignment
    t  3  clean apartment

## Usage

    flo [.tag |what[,from][-to] || [-c id] [-T tag] -w what [-f from | -t to] || -r id]

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

When changing an existing item, setting a date to `r` removes the date.

## Aliases

    # all todos
    alias ft='flo | grep ^t' 

    # all deadlines
    alias fd='flo | grep ^d' 

    # all events
    alias fe='flo | grep ^[^td]' 

    # today’s events and deadlines
    alias ftd='flo | grep today' 

    # tomorrow’s events and deadlines
    alias ftm='flo | grep tomorrow' 

## Items file

Items are written to ~/.flo. Each item is stored on a separate line, and each
field is separated by a tab.

    tag\twhat\tfrom\tto

## License

ISC.
