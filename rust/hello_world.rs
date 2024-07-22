#![no_std]
//#![feature(allocator_api, global_asm)]

use kernel::prelude::*;

module! {
    type: HelloWorld,
    name: "hello_world",
    author: "Your Name",
    description: "A simple hello world kernel module in Rust",
    license: "GPL",
}

struct HelloWorld;

impl kernel::Module for HelloWorld {
    fn init(_module: &'static ThisModule) -> Result<Self> {
        pr_info!("Hello, World!\n");
        Ok(HelloWorld)
    }
}

impl Drop for HelloWorld {
    fn drop(&mut self) {
        pr_info!("Goodbye, World!\n");
    }
}
