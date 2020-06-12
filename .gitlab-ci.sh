set -e
rm -rf .ciscript
git clone https://gitlab.vglab.es/common/ciscript.git .ciscript
bash .ciscript/gitlabci.sh
