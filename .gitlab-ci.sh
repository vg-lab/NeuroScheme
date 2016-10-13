set -e
rm -rf .ciscript
git clone https://gitlab.gmrv.es/common/ciscript.git .ciscript
bash .ciscript/gitlabci.sh
