cmd_/home/celeste/kernel2/test_receive.ko := ld -r -m elf_x86_64  -z max-page-size=0x200000  --build-id  -T ./scripts/module-common.lds -o /home/celeste/kernel2/test_receive.ko /home/celeste/kernel2/test_receive.o /home/celeste/kernel2/test_receive.mod.o;  true
