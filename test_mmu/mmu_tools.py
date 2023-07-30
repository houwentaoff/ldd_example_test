 #!/usr/bin/python3
import os
import sys
import json
import subprocess
import struct
import re
 
DEVICE_nGnRnE                  = 0
DEVICE_nGnRE                   = 1
NORMAL_NO_CACHEABLE            = 2
WRITE_BACK_TRANSIENT           = 3   # Write-Back Transient, Read Allocate, Write Allocate
WRITE_BACK_NON_TRANSIENT       = 4   # Write-Back Non-transient, Read Allocate, Write Allocate
MAIN_NORMAL_TAGGED             = 5   # Tagged Normal Memory. Inner+Outer Write-Back Non-Transient memory,Inner+Outer Read-Allocate, Inner+Outer Write-Allocate.
 
#
# Stage 1 Translation Table descriptor fields
#
TT_S1_ATTR_FAULT = (0b00 << 0)
TT_S1_ATTR_BLOCK = (0b01 << 0) #Level 1/2
TT_S1_ATTR_TABLE = (0b11 << 0) #Level 0/1/2
TT_S1_ATTR_PAGE  = (0b11 << 0) #Level 3
 
TT_S1_ATTR_MATTR_LSB = 2
 
TT_S1_ATTR_NS   = (1 << 5)
TT_S1_ATTR_S    = (0 << 5)
 
TT_S1_ATTR_AP_RW_PL1 = (0b00 << 6)
TT_S1_ATTR_AP_RW_ANY = (0b01 << 6)
TT_S1_ATTR_AP_RO_PL1 = (0b10 << 6)
TT_S1_ATTR_AP_RO_ANY = (0b11 << 6)
 
TT_S1_ATTR_SH_NONE   = (0b00 << 8)
TT_S1_ATTR_SH_OUTER  = (0b10 << 8)
TT_S1_ATTR_SH_INNER  = (0b11 << 8)
 
TT_S1_ATTR_AF = (1 << 10)
TT_S1_ATTR_nG = (1 << 11)
'''
4K : 512
16k:2048
64k:8192
'''
granularity={"4K":512, "16K":2048, "64K":8192}
attr={'device':0x1, 'cacheable':0x2, 'uncacheable':0x3}
attrs={
        'device':(DEVICE_nGnRnE << TT_S1_ATTR_MATTR_LSB) | \
        TT_S1_ATTR_AP_RW_PL1 | \
        TT_S1_ATTR_AF | \
        TT_S1_ATTR_nG,
        'cacheable':(WRITE_BACK_NON_TRANSIENT << TT_S1_ATTR_MATTR_LSB) | \
                TT_S1_ATTR_AP_RW_PL1 | \
                TT_S1_ATTR_SH_INNER | \
                TT_S1_ATTR_AF | \
                TT_S1_ATTR_nG,
        'uncacheable':(NORMAL_NO_CACHEABLE << TT_S1_ATTR_MATTR_LSB) | \
                TT_S1_ATTR_AP_RW_PL1 | \
                TT_S1_ATTR_SH_INNER | \
                TT_S1_ATTR_AF | \
                TT_S1_ATTR_nG
        }
 
class MMU_table(object):
    def __init__(self):
        self.entry_idx = 0
        # item entry 填写的地址 table为相对地址, block 为物理地址，
        self.entry_addr = 0
        #relative addr . 该item的相对地址
        self.rel_addr = 0x0
        #absolute addr
        #self.abs_addr = 0x0
        #self.phyaddr = 0x0
        self.attr = 0x0
        self.mem_attr = 0x0
        self.use = False
        self.type = "l1"
    def __mul__(self, other):
        l=[]
        n=other
        for i in range(n):
            l.append(MMU_table())
        return l
 
class MMU_tablel3(object):
    TT_S1_ATTR_FAULT = 0x0
    TT_S1_ATTR_BLOCK = 0x1
    TT_S1_ATTR_TABLE = 0x3
    TT_S1_ATTR_PAGE  = 0x3
    rel_addr = 0
    items=[]
 
    def __init__(self, elf, gran="4K"):
        if False == granularity.__contains__(gran):
            print("granularity is invalid!")
            exit(1)
        entry_num = granularity[gran]
        self.gran = gran
        self.entry_num = entry_num
 
        self.mmu_table = {"l1":MMU_table() * entry_num, "l2":MMU_table()* (entry_num*entry_num), "l3":MMU_table() * (8*entry_num*entry_num) }
 
        for table in self.mmu_table["l1"]:
            table.rel_addr = self.rel_addr
 
            self.items.append(table)
            self.rel_addr += 8
        '''     
        for table in self.mmu_table["l2"]:
            table.phyaddr = addr + 8
            addr += 8
        for table in self.mmu_table["l3"]:
            table.phyaddr = addr + 8
            addr += 8
        '''
    def align(self, addr, ali):
        addr = addr - (addr % ali)
        return addr
    '''
    #define TT_S1_ATTR_FAULT (0b00 << 0)
    #define TT_S1_ATTR_BLOCK (0b01 << 0) // Level 1/2
    #define TT_S1_ATTR_TABLE (0b11 << 0) // Level 0/1/2
    #define TT_S1_ATTR_PAGE  (0b11 << 0) // Level 3
    '''
    @staticmethod
    def getmmubase_fromelf(elf, elx):
        if elx == "el3":
            return MMU_tablel3.getsymaddr_fromelf(elf, "el3_mmu_start", False)
        if elx == "el1":
            return MMU_tablel3.getsymaddr_fromelf(elf, "el1_mmu_start", False)
 
    @staticmethod
    def getmmusize_fromelf(elf, elx):
        if elx == "el3":
            return MMU_tablel3.getsymaddr_fromelf(elf, "el3_mmu_end", True) - MMU_tablel3.getsymaddr_fromelf(elf, "el3_mmu_start", True)
        if elx == "el1":
            return MMU_tablel3.getsymaddr_fromelf(elf, "el1_mmu_end", True) - MMU_tablel3.getsymaddr_fromelf(elf, "el1_mmu_start", True)
    @staticmethod
    def getsymaddr_fromelf(elf, symbol, debug):
        cmd="readelf -a " + elf + "|grep \'" + symbol + "\'|awk '{print $2}'"
        #cmd="readelf -a " + elf + "|grep \'" + symbol + "  \'|awk '{print $4}'"  #mmu_table
        addr = subprocess.getoutput(cmd)
        addr = '0x' + addr
        if debug == True:
            print(symbol, ":", addr)
 
        return int(addr, 16)
    def align_lx(self, lx, addr):
        ln={"l1":30, "l2":21, "l3":12}
        return addr >> ln[lx] << ln[lx]
 
    def dump_tables(self):
        print("=>dump tables:")
        for t1 in self.mmu_table['l1']:
            virt = self.mmu_table['l1'].index(t1) * (1<<30)
            if (t1.attr == self.TT_S1_ATTR_BLOCK and t1.entry_addr != 0) or t1.use == True:
                if t1.attr == self.TT_S1_ATTR_BLOCK:
                    #print('G block:', hex(virt), "-->", hex(t1.entry_addr))
                    pass
                if t1.attr == self.TT_S1_ATTR_TABLE:
                    #print('G table:', hex(virt), "-->", hex(t1.entry_addr))
                    pass
                   
            if t1.attr == self.TT_S1_ATTR_TABLE:
                for i in range(0, 512):
                    t2 =  self.mmu_table['l2'][t1.entry_idx+i]
                    virt = self.mmu_table['l2'].index(t2) * (1<<21)
                    if t2.attr == self.TT_S1_ATTR_BLOCK and t2.entry_addr != 0:
                        #print('M block:', hex(virt), "-->", hex(t2.entry_addr))
                        pass
                    if t2.attr == self.TT_S1_ATTR_TABLE:
                        for j in range(0, 512):
                            t3 =  self.mmu_table['l3'][t2.entry_idx+j]
                            virt = self.mmu_table['l3'].index(t3) * (1<<12)
                            if t3.entry_addr!=0:
                                #print('K pagetable:', hex(virt), "-->", hex(t3.entry_addr))
                                pass
        print("<=dump tables:")
                   
 
    def write2bin(self, addr, elx, elf):
        di=os.path.dirname(elf) + "/"
        outfile = di + 'mmu' + elx + '.bin'
        print('write 2 addr[', hex(addr), '][', outfile, ']')
       
        b = b''
        for t in self.items:
            check = False
            # is l3?
            if t.type == "l3":
                if t.attr == self.TT_S1_ATTR_PAGE:
                    b += struct.pack('<Q', t.entry_addr | t.attr | t.mem_attr)
                    check = True
            else:       
                if t.attr == self.TT_S1_ATTR_TABLE:
                    b += struct.pack('<Q', t.entry_addr + addr | t.attr | t.mem_attr)
                    check = True
 
            if t.attr == self.TT_S1_ATTR_BLOCK:
                b += struct.pack('<Q', t.entry_addr | t.attr | t.mem_attr)
                check = True
            if t.attr == self.TT_S1_ATTR_FAULT:
                b += struct.pack('<Q', t.entry_addr | t.attr | t.mem_attr)
                check = True
 
            if check == False:
                print("t attr err? entry addr", t.entry_addr)
                exit(1)
 
        out  = open(outfile, "wb+")
        mmusize = MMU_tablel3.getmmusize_fromelf(elf, elx)
        print("mmudata", elx, " size ", len(b)/1024, "K")
        if len(b) > mmusize:
            print("[err]mmu section size ", hex(mmusize), " < ", hex(len(b)))
            exit(1)
        out.write(b)
        out.flush()
        out.close()
    def realloc_l2(self, g, m, blocksize):
        m = m - (m%512)
        #print("realloc_l2,", self.mmu_table["l1"][g].attr)
        if self.mmu_table["l1"][g].attr == 0 or self.mmu_table["l1"][g].attr == self.TT_S1_ATTR_BLOCK:
        #new 512
            for i in range(0, blocksize):
                if self.mmu_table["l1"][g].attr == self.TT_S1_ATTR_BLOCK:
                    self.mmu_table["l2"][m+i].attr = self.TT_S1_ATTR_BLOCK
                    self.mmu_table["l2"][m+i].mem_attr = self.mmu_table["l1"][g].mem_attr
                    self.mmu_table["l2"][m+i].entry_addr = self.mmu_table["l1"][g].entry_addr + i*(1<<21)
                else:
                    self.mmu_table["l2"][m+i].attr = self.TT_S1_ATTR_FAULT
                self.mmu_table["l2"][m+i].rel_addr = self.rel_addr
                self.mmu_table["l2"][m+i].type = "l2"
 
                self.items.append(self.mmu_table["l2"][m+i])
                self.rel_addr += 8
 
            self.mmu_table["l1"][g].entry_addr = self.mmu_table["l2"][m].rel_addr
            self.mmu_table["l1"][g].entry_idx = m;
            self.mmu_table["l1"][g].attr = self.TT_S1_ATTR_TABLE
        pass
    def realloc_l3(self, m, k, blocksize):
        #print("realloc_l3,", self.mmu_table["l2"][m].attr)
        if self.mmu_table["l2"][m].attr == 0 or self.mmu_table["l2"][m].attr == self.TT_S1_ATTR_BLOCK:
            #new 512
            for i in range(0, blocksize):
                if self.mmu_table["l2"][m].attr == self.TT_S1_ATTR_BLOCK:
                    self.mmu_table["l3"][k+i].attr = self.TT_S1_ATTR_PAGE
                    self.mmu_table["l3"][k+i].mem_attr = self.mmu_table["l2"][m].mem_attr
                    self.mmu_table["l3"][k+i].entry_addr = self.mmu_table["l2"][m].entry_addr + i*(1<<12)
                else:
                    self.mmu_table["l3"][k+i].attr = self.TT_S1_ATTR_FAULT
                self.mmu_table["l3"][k+i].rel_addr = self.rel_addr
                self.mmu_table["l3"][k+i].type = "l3"
 
                self.items.append(self.mmu_table["l3"][k+i])
                self.rel_addr += 8
 
            self.mmu_table["l2"][m].entry_addr = self.mmu_table["l3"][k].rel_addr
            self.mmu_table["l2"][m].entry_idx = k;
            self.mmu_table["l2"][m].attr = self.TT_S1_ATTR_TABLE
 
        pass
    def realloctb(self, begin, end, gran):
        if gran == "2M":
            begin = self.align(begin, 1<<30)
        if gran == "4K":
            begin = self.align(begin, 1<<21)
 
        while begin < end:
            (g,m,k) = (begin>>30, begin>>21, begin>>12)
            #print("=>realloctb begin:", hex(begin), ",g:", g, ",m", m, ",k:", k)
            #m -> 2M begin align
            if gran == "1G":
                begin += 1<<30
            if gran == "2M":
                self.realloc_l2(g, m, 512)
                begin += 1<<30
            if gran == "4K":
                self.realloc_l2(g, m, 512) #m:1G align
                self.realloc_l3(m, k, 512) #m:2M align
                begin += 1<<21
 
    def getattrstr(self, attr):
        attr = attr & ~TT_S1_ATTR_NS
        for k in attrs:
            if attr == attrs[k]:
                return k
        return str("unknow attr")
 
    def mmumap(self, phy_begin, phy_end, virt_begin, gran, attr):
 
        print("mmap phybegin ", hex(phy_begin), "-->", hex(phy_end), ", attr[", self.getattrstr(attr), ":", hex(attr), "]")
 
        self.realloctb(virt_begin, virt_begin + (phy_end-phy_begin), gran)
 
        start = virt_begin
        end = phy_end - phy_begin + virt_begin
        entry = 0x0
        phy_addr = phy_begin
 
        while start < end:
            #(g, m, k) = self.finditembegin(start)
            (g, m, k) = (start>>30, start>>21, start>>12)
            #print("mmumap,g:", g, ",m:", m, ",k:", k)
 
            if gran == "1G":
               entry_addr = self.align_lx("l1", phy_addr)
                self.mmu_table["l1"][g].entry_addr = entry_addr
                self.mmu_table["l1"][g].attr = self.TT_S1_ATTR_BLOCK
                self.mmu_table["l1"][g].mem_attr = attr
                self.mmu_table["l1"][g].use = True
                start    += 1<<30
                phy_addr += 1<<30
            if gran == "2M":
                entry = self.align_lx("l2", phy_addr)
                self.mmu_table["l2"][m].entry_addr = entry
                self.mmu_table["l2"][m].attr = self.TT_S1_ATTR_BLOCK
                self.mmu_table["l2"][m].mem_attr = attr
                self.mmu_table["l2"][m].use = True
                start    += 1<<21
                phy_addr += 1<<21
            if gran == "4K":
                entry = self.align_lx("l3", phy_addr)
                self.mmu_table["l3"][k].entry_addr = entry
                self.mmu_table["l3"][k].attr = self.TT_S1_ATTR_PAGE
                self.mmu_table["l3"][k].mem_attr = attr
                self.mmu_table["l3"][k].use = True
                start    += 1<<12
                phy_addr += 1<<12
       
def findelx(diclist, x):
    ml = []
    for m in diclist:
        if x == m['level']:
            ml = m['map']
            break
    return (x, ml)
 
def gen_elxbin(elx, mmu_dic, elf):
    (elx, maplist) = findelx(mmu_dic['mmucfg'], elx)
    tables = MMU_tablel3(elf, "4K")
    for m in maplist:
        if elx == "el1":
            tables.mmumap(int( m['phy-addr-begin'] ,16),
                      int( m['phy-addr-end'], 16),
                      int( m['virt-begin'], 16),
                      m['granularity'],
                      attrs[m['attr']] | TT_S1_ATTR_NS)
        else:   
            tables.mmumap(int( m['phy-addr-begin'] ,16),
                      int( m['phy-addr-end'], 16),
                      int( m['virt-begin'], 16),
                      m['granularity'],
                      attrs[m['attr']])
 
    mmu_elx_addr = MMU_tablel3.getmmubase_fromelf(elf, elx)    
    print('mmu ', elx, ' addr:', hex(mmu_elx_addr))
    #tables.dump_tables()
    tables.write2bin(mmu_elx_addr, elx, elf)               
    #mmu_el1_addr = mmu_el3_addr + MMU_tablel3.getsize()
    #tables.write2bin(mmu_el3_addr, "el3")                
# 创建一个xstr类，用于处理从文件中读出的字符串
class xstr:
    def __init__(self, instr):
        self.instr = instr
 
    # 删除“//”标志后的注释
    def rmCmt(self):
        qtCnt = cmtPos = slashPos = 0
        rearLine = self.instr
        # rearline: 前一个“//”之后的字符串，
        # 双引号里的“//”不是注释标志，所以遇到这种情况，仍需继续查找后续的“//”
        while rearLine.find('//') >= 0: # 查找“//”
            slashPos = rearLine.find('//')
            cmtPos += slashPos
            # print 'slashPos: ' + str(slashPos)
            headLine = rearLine[:slashPos]
            while headLine.find('"') >= 0: # 查找“//”前的双引号
                qtPos = headLine.find('"')
                if not self.isEscapeOpr(headLine[:qtPos]): # 如果双引号没有被转义
                    qtCnt += 1 # 双引号的数量加1
                headLine = headLine[qtPos+1:]
                # print qtCnt
            if qtCnt % 2 == 0: # 如果双引号的数量为偶数，则说明“//”是注释标志
                # print self.instr[:cmtPos]
                return self.instr[:cmtPos]
            rearLine = rearLine[slashPos+2:]
            # print rearLine
            cmtPos += 2
        # print self.instr
        return self.instr
 
    # 判断是否为转义字符
    def isEscapeOpr(self, instr):
        if len(instr) <= 0:
            return False
        cnt = 0
        while instr[-1] == '\\':
            cnt += 1
            instr = instr[:-1]
        if cnt % 2 == 1:
            return True
        else:
            return False
 
 
# 从json文件的路径JsonPath读取该文件，返回json对象
def load_json(JsonPath):
    try:
        srcJson = open(JsonPath, 'r')
    except:
        print(CAUTION_PRINT_HEAD + 'cannot open ' + JsonPath)
        quit()
 
    dstJsonStr = ''
    for line in srcJson.readlines():
        if not re.match(r'\s*//', line) and not re.match(r'\s*\n', line):
            xline = xstr(line)
            dstJsonStr += xline.rmCmt()
 
    # print dstJsonStr
    dstJson = {}
    try:
        dstJson = json.loads(dstJsonStr)
        return dstJson
    except:
        print(CAUTION_PRINT_HEAD + JsonPath + ' is not a valid json file')
        quit()
if len(sys.argv) < 3:
    print("eg:mmu_tool.py mmu.json test_mmu.elf")
    exit(1)
'''
for argv in sys.argv:
    print("argv:", argv)
'''
 
mmucfg = sys.argv[1]
elf    = sys.argv[2]
mmu_dic={}
mmu_dic = load_json(mmucfg)
#with open(mmucfg, 'r', encoding='utf-8') as f:
#        mmu_dic = load_json(f)#json.load(f)
        #print(type(mmu_dic)) # 结果 <class 'dict'>
#print(mmu_dic)
gen_elxbin("el3", mmu_dic, elf)
gen_elxbin("el1", mmu_dic, elf)
