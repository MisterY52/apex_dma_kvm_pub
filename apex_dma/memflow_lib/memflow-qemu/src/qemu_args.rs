pub fn is_qemu(process: &memflow::os::process::ProcessInfo) -> bool {
    let name = &*process.name;
    name.contains("qemu-system-") || name == "QEMULauncher" || name == "kvm"
}

pub fn qemu_arg_opt<'a>(
    args: impl IntoIterator<Item = &'a str>,
    argname: &str,
    argopt: &str,
) -> Option<String> {
    let mut iter = args.into_iter().peekable();

    while let (Some(arg), Some(next)) = (iter.next(), iter.peek()) {
        if arg == argname {
            let name = next.split(',');
            for (i, kv) in name.clone().enumerate() {
                let kvsplt = kv.split('=').collect::<Vec<_>>();
                if kvsplt.len() == 2 {
                    if kvsplt[0] == argopt {
                        return Some(kvsplt[1].to_string());
                    }
                } else if i == 0 {
                    return Some(kv.to_string());
                }
            }
        }
    }

    None
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_name() {
        assert_eq!(
            qemu_arg_opt(["-name", "win10-test"].iter().copied(), "-name", "guest"),
            Some("win10-test".into())
        );
        assert_eq!(
            qemu_arg_opt(
                ["-test", "-name", "win10-test"].iter().copied(),
                "-name",
                "guest"
            ),
            Some("win10-test".into())
        );
        assert_eq!(
            qemu_arg_opt(
                ["-name", "win10-test,arg=opt"].iter().copied(),
                "-name",
                "guest"
            ),
            Some("win10-test".into())
        );
        assert_eq!(
            qemu_arg_opt(
                ["-name", "guest=win10-test,arg=opt"].iter().copied(),
                "-name",
                "guest"
            ),
            Some("win10-test".into())
        );
        assert_eq!(
            qemu_arg_opt(
                ["-name", "arg=opt,guest=win10-test"].iter().copied(),
                "-name",
                "guest"
            ),
            Some("win10-test".into())
        );
        assert_eq!(
            qemu_arg_opt(["-name", "arg=opt"].iter().copied(), "-name", "guest"),
            None
        );
    }

    #[test]
    fn test_machine() {
        assert_eq!(
            qemu_arg_opt(["-machine", "q35"].iter().copied(), "-machine", "type"),
            Some("q35".into())
        );
        assert_eq!(
            qemu_arg_opt(
                ["-test", "-machine", "q35"].iter().copied(),
                "-machine",
                "type"
            ),
            Some("q35".into())
        );
        assert_eq!(
            qemu_arg_opt(
                ["-machine", "q35,arg=opt"].iter().copied(),
                "-machine",
                "type"
            ),
            Some("q35".into())
        );
        assert_eq!(
            qemu_arg_opt(
                ["-machine", "type=pc,arg=opt"].iter().copied(),
                "-machine",
                "type"
            ),
            Some("pc".into())
        );
        assert_eq!(
            qemu_arg_opt(
                ["-machine", "arg=opt,type=pc-i1440fx"].iter().copied(),
                "-machine",
                "type"
            ),
            Some("pc-i1440fx".into())
        );
        assert_eq!(
            qemu_arg_opt(["-machine", "arg=opt"].iter().copied(), "-machine", "type"),
            None
        );
    }
}
