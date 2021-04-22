Qucs-S modules for use with "modular Qucs".

This repository shows how Qucs-S will work. Implementation details may vary
over time.

== requirements

This will only work with modular Qucs (modular branch, watch out for a
development snapshot). The instruction below require it to be installed (as in
"make install").

== build

It is currently required to build upfront. Qucs will deal with this at a later
stage. Just type.

$ make

== run

$ qucscli < qucs-s.qx

== TODO

- add tests
- add a netlister
- add a xyce driver
