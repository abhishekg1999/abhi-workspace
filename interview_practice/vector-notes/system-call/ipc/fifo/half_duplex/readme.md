#:

Writer                          Reader

mkfifo()

open(O_WRONLY)

      waiting...

                           open(O_RDONLY)

open() returns

scanf()

write()

---------------------------->

                         read()

                         printf()

(repeat)
