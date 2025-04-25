from building import *
import rtconfig

# 获取当前目录
cwd     = GetCurrentDir()
# 定义源文件
src     = Glob('*.c')
CPPPATH = [cwd]

# 定义HSM最大嵌套层级
if GetDepend('PKG_HSM_MAX_LEVEL'):
    HSM_MAX_LEVEL = GetConfigValue('PKG_HSM_MAX_LEVEL')
    LOCAL_CCFLAGS = ' -DHSM_MAX_LEVEL=' + HSM_MAX_LEVEL

# 添加示例代码
if GetDepend('PKG_HSM_USING_EXAMPLE'):
    src    += Glob('examples/led/*.c')
    CPPPATH += [cwd + '/examples/led']

# 生成组件
group = DefineGroup('hsm', src, depend = ['PKG_USING_HSM'], CPPPATH = CPPPATH, LOCAL_CCFLAGS = LOCAL_CCFLAGS)

Return('group') 