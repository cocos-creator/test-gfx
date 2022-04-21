# test-gfx

a light-weight repo used to test the feature of the _gfx_ module of cocos-creator.

## Build

_test_gfx_ should be put alongside the engine repo:

<pre>
├── cocos-engine (<a href="https://github.com/cocos/cocos-engine/tree/v3.5.0">3.5.0</a>)
│   └── native/external (<a href="https://github.com/cocos/cocos-engine-external/tree/v3.5">3.5</a>)
└── test-gfx (<a href="https://github.com/cocos-creator/test-gfx/tree/master">master</a>)
</pre>

### web

Enter ./engine folder and run the following commands
```bash
    $ ./util.sh update       # build the project.
    $ ./util.sh server start # start http server and run test cases in your browser.
```
You can also run `./util.sh help` to see the help message.
> Note: you only need to start the server at the beginning. If you can't start the server, please run `./util.sh server stop` and then `./util.sh server start`.

### native
1. Windows
    
    In the root directory, run the following commands

    ```bash
    $ cmake -Bbuild # config the project
    $ cmake --build build # build the project
    ```

    After configuring the project, you can also open `build/test-gfx.sln` with _Visual Studio_.

2. Android
    * In _Android Studio_, import the project in `./src/android/`.
    * Build the project.