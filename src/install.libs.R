### Modified from Rserve/src/install.libs.R
### For libs
files <- c("rzmq.so", "rzmq.so.dSYM", "rzmq.dylib", "rzmq.dll",
           "symbols.rds")
files <- files[file.exists(files)]
if(length(files) > 0){
  libsarch <- if (nzchar(R_ARCH)) paste("libs", R_ARCH, sep='') else "libs"
  dest <- file.path(R_PACKAGE_DIR, libsarch)
  dir.create(dest, recursive = TRUE, showWarnings = FALSE)
  file.copy(files, dest, overwrite = TRUE, recursive = TRUE)

  ### For Mac OSX 10.10 Yosemite and when "internal ZMQ" is asked.
  ### Overwrite RPATH from the shared library installed to the destination.
  if(Sys.info()[['sysname']] == "Darwin"){
    cmd.int <- system("which install_name_tool", intern = TRUE)
    fn.rzmq.so <- file.path(dest, "rzmq.so")

    zmq.ldflags <- pbdZMQ:::get.zmq.ldflags()
    dest.zmq <- gsub("-L(.*) -l.*", "\\1", zmq.ldflags)
    fn.libzmq.4.dylib <- file.path(dest.zmq, "libzmq.4.dylib")

    if(length(grep("install_name_tool", cmd.int)) > 0 &&
       file.exists(fn.rzmq.so) &&
       file.exists(fn.libzmq.4.dylib)){

      cmd.ot <- system("which otool", intern = TRUE) 
      if(length(grep("otool", cmd.ot)) > 0){
        rpath <- system(paste(cmd.ot, " -L ", fn.rzmq.so, sep = ""),
                        intern = TRUE)
        cat("\nBefore install_name_tool:\n")
        print(rpath)

	id <- grep("libzmq.4.dylib", rpath)
        org <- gsub("\\t(.*) \(.*\)", "\\1", rpath[id])
        cmd <- paste(cmd.int, " -change ", org, " ", fn.libzmq.4.dylib, " ",
                     fn.rzmq.so, sep = "")
        cat("\nIn install_name_tool:\n")
        print(cmd) 
        system(cmd)

        rpath <- system(paste(cmd.ot, " -L ", fn.rzmq.so, sep = ""),
                        intern = TRUE)
        cat("\nAfter install_name_tool:\n")
        print(rpath)
      }
    }
  }
}

