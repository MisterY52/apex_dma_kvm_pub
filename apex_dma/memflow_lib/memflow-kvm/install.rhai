fn install_message() {
    info("memflow-kvm kernel module installation finished");
    info("you might have to manually invoke 'modprobe memflow' after installation and make sure the /dev/memflow device is present and you have access to it.");
    info("it is also recommended to setup a 'memflow' user group in linux and add your user to it");
    info("additionally, you might want to setup a udev rule thatll make /dev/memflow accessible to that group.");
    info("see the readme for more details");
}

// builds the connector from a given path (never called by the engine directly)
fn build_from_path(ctx, repo_path) {
    info("Installing connector");
    cargo("build --release --all-features", repo_path);
    ctx.copy_cargo_plugin_artifact(repo_path, name_to_lib(ctx.crate_name()));

    info("Installing memflow-kvm kernel module");
    ctx.dkms_install(repo_path);

    post_install();

    install_message();
}

// builds the connector from local path
fn build_local(ctx) {
    build_from_path(ctx, ctx.build_path())
}

fn get_source(ctx) {
    // clone the repository with all submodules via git
    ctx.clone_repository()
}

// builds the connector from source
fn build_from_source(ctx) {
    build_from_path(ctx, get_source(ctx))
}

// downloads a binary release of the plugin, still needs dkms
fn install(ctx) {
    let libname = name_to_lib_with_arch(ctx.crate_name());
    let artifact = ctx.github_release_artifact(libname);
    ctx.write_plugin_artifact(artifact, name_to_lib(ctx.crate_name()));

    info("Installing memflow-kvm kernel module");
    let tarball = ctx.github_release_artifact("memflow-source-only.dkms.tar.gz");
    ctx.dkms_install(tarball);

    post_install();

    install_message();
}

fn post_install() {
    try {
        udev_add_rule("memflow-kvm", "KERNEL==\"memflow\" SUBSYSTEM==\"misc\" GROUP=\"memflow\" MODE=\"0660\"");
    } catch (err) {
        info("Your memflowup installation might be out of date, please make sure to use the latest version.")
    }
}
