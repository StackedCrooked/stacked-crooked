#!/bin/bash
ssh root@bb-dev-3200-1.lab.bb.e.com 'cd /root/DPDKSamples && ./compile.sh && echo "Exit code $?"' 2>err; cat err | sed -e 's,/root/DPDKSamples/,,' >&2
#ssh root@bb-dev-3200-1.lab.bb.e.com 'cd /root/DPDKSamples/rxtx_callbacks && ./compile.sh && echo "Exit code $?"' 2>err; cat err | sed -e 's,/root/DPDKSamples/,,' >&2
