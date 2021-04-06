set -e
rm -rf .ciscript
git clone https://gitlab.vg-lab.es/common/ciscript.git .ciscript
bash .ciscript/gitlabci.sh
