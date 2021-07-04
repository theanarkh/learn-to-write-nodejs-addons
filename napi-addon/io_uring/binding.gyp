{
    "targets": [
        {
            "target_name": "io_uring",
            "sources": [ "./io_uring.cc" ] ,
            "libraries": ["<(module_root_dir)/liburing.a"]
        }
    ]
}