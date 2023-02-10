
- Shaders files end with ".fs" or ".vs".

- ".fs": fragment shader file, ".vs": vertex shader file.

- ".sub_fs" or ".sub_vs" is **SubShader** file, which can be **import** into ".main_fs" or ".main_vs" to generate final ".fs" or ".vs"

- ".main_fs" or ".main_vs" can use `#import:"sub shader path"#` to import sub shader. For example, if we have a sub shader file "refer.sub_fs" in folder "/Resources/SubShaders/", then we can write `#import:"SubShaders/refer.sub_fs"#` in a ".main_fs" file. The final output shader file must be specified.(explained below "outputs")

- Using command `load_scene xxx.json` can possibly generate shader files. Here is an example for such ".json" files:
```
{
	"generate_shader": {
                        "inputs": ["Test/input.main_fs", "Test/input.main_vs"],
                        "outputs": ["Test/output.fs", "Test/output.vs"]
                       }
}
```
"outputs" is used to specify the final shader file name.