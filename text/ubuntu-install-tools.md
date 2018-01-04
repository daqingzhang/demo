Install Tools for Ubuntu
==================================================

1. install software tools
--------------------------------------------------
sudo apt-get install git git-doc git-man
sudo apt-get install vim vim-doc vim-scripts
sudo apt-get install ctags
sudo apt-get install terminator
sudo apt-get install minicom
sudo apt-get install fastboot

2. install sunpinyin
--------------------------------------------------

sudo apt-get install ibus-sunpinyin
sudo apt-get install ibus-libpinyin
sudo apt-get install ibus-pinyin

reboot your PC


3. configure vim
--------------------------------------------------
cd ~/.

vi .vimrc file:
	syntax on
	set number
	set smartindent
	set tags=tags;
	colorscheme evening
	highlight Pmenu guibg=brown gui=bold


mkdir -p .vim/doc
mkdir -p .vim/plugin

cp -rp /usr/share/vim-scripts/doc/taglist.txt .vim/doc/
cp -rp /usr/share/vim-scripts/doc/bufexplorer.txt .vim/doc/

cp -rp /usr/share/vim-scripts/plugin/taglist.vim .vim/plugin/
cp -rp /usr/share/vim-scripts/plugin/minibufexpl.vim .vim/plugin/


4. configure git
--------------------------------------------------
cd ~/.
vi .gitconfig:
	[user]
		name = daqingzhang
		email = daqingzhang@rdamicro.com
	[core]
		editor = vim
	[color]
		ui = auto
