#make sure that you have built a remote repositorie already
git init
git add -A
git commit -m "旧开发板网络转发车功能需存档"

git remote add origin git@github.com:sundajiang/ap_net.git
git push -u origin master
