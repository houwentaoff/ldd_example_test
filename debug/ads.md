# debug configurations页面 
execute debuger commands 中填写如下配置即可在el1 el3中正确加载使用symbol方便调试
add-symbol-file "${workspace_loc:/sv2021/bin/case.elf}" EL1N:0
add-symbol-file "${workspace_loc:/sv2021/bin/case.elf}" EL3:0
