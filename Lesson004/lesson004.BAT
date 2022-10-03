SET PATH=C:\Keil\ARM\ARMCC\bin\;C:\Program Files\Common Files\Oracle\Java\javapath;C:\Program Files\Common Files\Microsoft Shared\Windows Live;C:\Program Files (x86)\Common Files\Microsoft Shared\Windows Live;C:\gradle-2.0\bin;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\Program Files (x86)\Windows Live\Shared;C:\WINDOWS\System32\OpenSSH\;C:\Program Files\Git\cmd;C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility;C:\CooCox\CoIDE\gcc\bin;C:\Program Files\Microchip\xc8\v2.31\bin;C;C:\Users\Alex Ch\AppData\Local\Programs\Python\Python39\Scripts\;C:\Users\Alex Ch\AppData\Local\Programs\Python\Python39\;C:\Users\Alex Ch\AppData\Local\Programs\Python\Python38\Scripts\;C:\Users\Alex Ch\AppData\Local\Programs\Python\Python38\;C:\Users\Alex_Ch\AppData\Local\Microsoft\WindowsApps;C:\Users\Alex Ch\AppData\Local\Programs\Microsoft VS Code\bin;C:\Program Files\JetBrains\IntelliJ IDEA Community Edition 2021.3.2\bin;;C:\Users\Alex Ch\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Python 3.9;C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Git\bin;C:\Program Files (x86)\COSMIC\FSE_Compilers\CXSTM8;C:\Program Files (x86)\COSMIC\FSE_Compilers\CXSTM32
SET CPU_TYPE=SC000
SET CPU_VENDOR=HED
SET UV2_TARGET=CIU98428F_DemoCOS
SET CPU_CLOCK=0x01AB3F00
"C:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\obj\startup._ia"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\main.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\demo_apdu.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\demo_subfunc.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\ciu98428f_api_base_nonvm.__i"
"C:\Keil\ARM\ARMCC\bin\ArmCC" --Via ".\obj\ciu98428f_api_nvm_stability.__i"
"C:\Keil\ARM\ARMCC\bin\ArmAsm" --Via ".\obj\ciu98428f_nvm_stability._ia"
"C:\Keil\ARM\ARMCC\bin\ArmLink" --Via ".\obj\CIU98428F_DemoCOS.lnp"
"C:\Keil\ARM\ARMCC\bin\fromelf.exe" ".\obj\CIU98428F_DemoCOS.axf" --i32combined --output ".\obj\CIU98428F_DemoCOS.hex"
