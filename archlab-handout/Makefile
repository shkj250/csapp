####################################################
# Students' Makefile for the CS:APP Architecture Lab
# 中文：CS:APP 体系结构实验的学生 Makefile
####################################################

# Default team name and version number
# 中文：默认团队名称和版本号
TEAM = bovik
VERSION = 1

# Where are the different parts of the lab should be copied to when they
# 中文：以下指定实验各部分在提交时应复制到的位置
# are handed in.
# 中文：即各部分的提交目录
HANDINDIR-PARTA = /afs/cs/academic/class/15349-s02/archlab/handin-parta
HANDINDIR-PARTB = /afs/cs/academic/class/15349-s02/archlab/handin-partb
HANDINDIR-PARTC = /afs/cs/academic/class/15349-s02/archlab/handin-partc

sim:
	(cd sim; make)

# Use this rule to hand in Part A ("make handin-parta")
# 中文：使用本规则提交 Part A：make handin-parta
handin-parta:
	cp sim/misc/sum.ys $(HANDINDIR-PARTA)/$(TEAM)-$(VERSION)-sum.ys
	cp sim/misc/rsum.ys $(HANDINDIR-PARTA)/$(TEAM)-$(VERSION)-rsum.ys
	cp sim/misc/copy.ys $(HANDINDIR-PARTA)/$(TEAM)-$(VERSION)-copy.ys

# Use this rule to handin Part B ("make handin-partb")
# 中文：使用本规则提交 Part B：make handin-partb
handin-partb:
	cp sim/seq/seq-full.hcl $(HANDINDIR-PARTB)/$(TEAM)-$(VERSION)-seq-full.hcl

# Use this rule to handin Part C ("make handin-partc")
# 中文：使用本规则提交 Part C：make handin-partc
handin-partc:
	cp sim/pipe/ncopy.ys $(HANDINDIR-PARTC)/$(TEAM)-$(VERSION)-ncopy.ys
	cp sim/pipe/pipe-full.hcl $(HANDINDIR-PARTC)/$(TEAM)-$(VERSION)-pipe-full.hcl

clean:
	rm -f *~ *.o



