SYSCALL_DEFINE4(reboot,
        int, magic1,
        int, magic2,
        unsigned int, cmd,
        void __user *, arg)
{
    char buffer[256];

    if (!capable(CAP_SYS_BOOT))
        return -EPERM;

    /* just for security */
    if (magic1 != LINUX_REBOOT_MAGIC1 ||
        (magic2 != LINUX_REBOOT_MAGIC2 &&
         magic2 != LINUX_REBOOT_MAGIC2A &&
         magic2 != LINUX_REBOOT_MAGIC2B &&
         magic2 != LINUX_REBOOT_MAGIC2C))
        return -EINVAL;

    if ((cmd == LINUX_REBOOT_CMD_POWER_OFF) && !pm_power_off)
        cmd = LINUX_REBOOT_CMD_HALT;

    lock_kernel();

    switch (cmd) {
    case LINUX_REBOOT_CMD_RESTART:
        kernel_restart(NULL);
        break;

    case LINUX_REBOOT_CMD_CAD_ON:
        C_A_D = 1;
        break;

    case LINUX_REBOOT_CMD_CAD_OFF:
        C_A_D = 0;
        break;

    case LINUX_REBOOT_CMD_HALT:
        kernel_halt();
        unlock_kernel();
        do_exit(0);
        break;

    case LINUX_REBOOT_CMD_POWER_OFF:
        kernel_power_off();
        unlock_kernel();
        do_exit(0);
        break;

    case LINUX_REBOOT_CMD_RESTART2:
        if (strncpy_from_user(&buffer[0], arg, sizeof(buffer)-1) < 0) {
            unlock_kernel();
            return -EFAULT;
        }
        buffer[sizeof(buffer)-1] = '\0';
        kernel_restart(buffer);
        break;

    default:
        unlock_kernel();
        return -EINVAL;
    }
    unlock_kernel();

    return 0;
}
