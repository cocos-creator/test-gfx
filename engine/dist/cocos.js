/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/descriptor-set-layout", ["engine/cocos/core/gfx/base/define"], function (exports_1, context_1) {
    "use strict";
    var define_1, DescriptorSetLayout;
    var __moduleName = context_1 && context_1.id;
    return {
        setters: [
            function (define_1_1) {
                define_1 = define_1_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX descriptor sets layout.
             * @zh GFX 描述符集布局。
             */
            DescriptorSetLayout = class DescriptorSetLayout extends define_1.GFXObject {
                constructor() {
                    super(define_1.ObjectType.DESCRIPTOR_SET_LAYOUT);
                    this._bindings = [];
                    this._bindingIndices = [];
                    this._descriptorIndices = [];
                }
                get bindings() {
                    return this._bindings;
                }
                get bindingIndices() {
                    return this._bindingIndices;
                }
                get descriptorIndices() {
                    return this._descriptorIndices;
                }
            };
            exports_1("DescriptorSetLayout", DescriptorSetLayout);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/utils/murmurhash2_gc", [], function (exports_2, context_2) {
    "use strict";
    var getUint8ForString;
    var __moduleName = context_2 && context_2.id;
    function getUint8ForArray(idx) { return this[idx]; }
    function murmurhash2_32_gc(input, seed) {
        let l = input.length;
        let h = seed ^ l;
        let i = 0;
        const getUint8 = typeof input === 'string' ? getUint8ForString : getUint8ForArray;
        while (l >= 4) {
            let k = ((getUint8.call(input, i) & 0xff))
                | ((getUint8.call(input, ++i) & 0xff) << 8)
                | ((getUint8.call(input, ++i) & 0xff) << 16)
                | ((getUint8.call(input, ++i) & 0xff) << 24);
            k = (((k & 0xffff) * 0x5bd1e995) + ((((k >>> 16) * 0x5bd1e995) & 0xffff) << 16));
            k ^= k >>> 24;
            k = (((k & 0xffff) * 0x5bd1e995) + ((((k >>> 16) * 0x5bd1e995) & 0xffff) << 16));
            h = (((h & 0xffff) * 0x5bd1e995) + ((((h >>> 16) * 0x5bd1e995) & 0xffff) << 16)) ^ k;
            l -= 4;
            ++i;
        }
        switch (l) {
            case 3: h ^= (getUint8.call(input, i + 2) & 0xff) << 16;
            case 2: h ^= (getUint8.call(input, i + 1) & 0xff) << 8;
            case 1:
                h ^= (getUint8.call(input, i) & 0xff);
                h = (((h & 0xffff) * 0x5bd1e995) + ((((h >>> 16) * 0x5bd1e995) & 0xffff) << 16));
        }
        h ^= h >>> 13;
        h = (((h & 0xffff) * 0x5bd1e995) + ((((h >>> 16) * 0x5bd1e995) & 0xffff) << 16));
        h ^= h >>> 15;
        return h >>> 0;
    }
    exports_2("murmurhash2_32_gc", murmurhash2_32_gc);
    return {
        setters: [],
        execute: function () {
            /*
             * JS Implementation of MurmurHash2
             *
             * @author <a href="mailto:gary.court@gmail.com">Gary Court</a>
             * @see http://github.com/garycourt/murmurhash-js
             * @author <a href="mailto:aappleby@gmail.com">Austin Appleby</a>
             * @see http://sites.google.com/site/murmurhash/
             *
             * @param {string} str ASCII only
             * @param {number} seed Positive integer only
             * @return {number} 32-bit positive integer hash
             */
            /**
             * @packageDocumentation
             * @hidden
             */
            getUint8ForString = String.prototype.charCodeAt;
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/render-pass", ["engine/cocos/core/utils/murmurhash2_gc", "engine/cocos/core/gfx/base/define"], function (exports_3, context_3) {
    "use strict";
    var murmurhash2_gc_1, define_2, RenderPass;
    var __moduleName = context_3 && context_3.id;
    return {
        setters: [
            function (murmurhash2_gc_1_1) {
                murmurhash2_gc_1 = murmurhash2_gc_1_1;
            },
            function (define_2_1) {
                define_2 = define_2_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX render pass.
             * @zh GFX 渲染过程。
             */
            RenderPass = class RenderPass extends define_2.GFXObject {
                constructor() {
                    super(define_2.ObjectType.RENDER_PASS);
                    this._colorInfos = [];
                    this._depthStencilInfo = null;
                    this._subpasses = [];
                    this._hash = 0;
                }
                get colorAttachments() { return this._colorInfos; }
                get depthStencilAttachment() { return this._depthStencilInfo; }
                get subPasses() { return this._subpasses; }
                get hash() { return this._hash; }
                // Based on render pass compatibility
                computeHash() {
                    let res = '';
                    if (this._subpasses.length) {
                        for (let i = 0; i < this._subpasses.length; ++i) {
                            const subpass = this._subpasses[i];
                            if (subpass.inputs.length) {
                                res += 'ia';
                                for (let j = 0; j < subpass.inputs.length; ++j) {
                                    const ia = this._colorInfos[subpass.inputs[j]];
                                    res += `,${ia.format},${ia.sampleCount}`;
                                }
                            }
                            if (subpass.colors.length) {
                                res += 'ca';
                                for (let j = 0; j < subpass.inputs.length; ++j) {
                                    const ca = this._colorInfos[subpass.inputs[j]];
                                    res += `,${ca.format},${ca.sampleCount}`;
                                }
                            }
                            if (subpass.depthStencil >= 0) {
                                const ds = this._colorInfos[subpass.depthStencil];
                                res += `ds,${ds.format},${ds.sampleCount}`;
                            }
                        }
                    }
                    else {
                        res += 'ca';
                        for (let i = 0; i < this._colorInfos.length; ++i) {
                            const ca = this._colorInfos[i];
                            res += `,${ca.format},${ca.sampleCount}`;
                        }
                        const ds = this._depthStencilInfo;
                        if (ds) {
                            res += `ds,${ds.format},${ds.sampleCount}`;
                        }
                    }
                    return murmurhash2_gc_1.murmurhash2_32_gc(res, 666);
                }
            };
            exports_3("RenderPass", RenderPass);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/texture", ["engine/cocos/core/gfx/base/define"], function (exports_4, context_4) {
    "use strict";
    var define_3, Texture;
    var __moduleName = context_4 && context_4.id;
    return {
        setters: [
            function (define_3_1) {
                define_3 = define_3_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX texture.
             * @zh GFX 纹理。
             */
            Texture = class Texture extends define_3.GFXObject {
                constructor() {
                    super(define_3.ObjectType.TEXTURE);
                    this._type = define_3.TextureType.TEX2D;
                    this._usage = define_3.TextureUsageBit.NONE;
                    this._format = define_3.Format.UNKNOWN;
                    this._width = 0;
                    this._height = 0;
                    this._depth = 1;
                    this._layerCount = 1;
                    this._levelCount = 1;
                    this._samples = define_3.SampleCount.ONE;
                    this._flags = define_3.TextureFlagBit.NONE;
                    this._isPowerOf2 = false;
                    this._size = 0;
                }
                /**
                 * @en Get texture type.
                 * @zh 纹理类型。
                 */
                get type() {
                    return this._type;
                }
                /**
                 * @en Get texture usage.
                 * @zh 纹理使用方式。
                 */
                get usage() {
                    return this._usage;
                }
                /**
                 * @en Get texture format.
                 * @zh 纹理格式。
                 */
                get format() {
                    return this._format;
                }
                /**
                 * @en Get texture width.
                 * @zh 纹理宽度。
                 */
                get width() {
                    return this._width;
                }
                /**
                 * @en Get texture height.
                 * @zh 纹理高度。
                 */
                get height() {
                    return this._height;
                }
                /**
                 * @en Get texture depth.
                 * @zh 纹理深度。
                 */
                get depth() {
                    return this._depth;
                }
                /**
                 * @en Get texture array layer.
                 * @zh 纹理数组层数。
                 */
                get layerCount() {
                    return this._layerCount;
                }
                /**
                 * @en Get texture mip level.
                 * @zh 纹理 mip 层级数。
                 */
                get levelCount() {
                    return this._levelCount;
                }
                /**
                 * @en Get texture samples.
                 * @zh 纹理采样数。
                 */
                get samples() {
                    return this._samples;
                }
                /**
                 * @en Get texture flags.
                 * @zh 纹理标识位。
                 */
                get flags() {
                    return this._flags;
                }
                /**
                 * @en Get texture size.
                 * @zh 纹理大小。
                 */
                get size() {
                    return this._size;
                }
            };
            exports_4("Texture", Texture);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/framebuffer", ["engine/cocos/core/gfx/base/define"], function (exports_5, context_5) {
    "use strict";
    var define_4, Framebuffer;
    var __moduleName = context_5 && context_5.id;
    return {
        setters: [
            function (define_4_1) {
                define_4 = define_4_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX frame buffer.
             * @zh GFX 帧缓冲。
             */
            Framebuffer = class Framebuffer extends define_4.GFXObject {
                constructor() {
                    super(define_4.ObjectType.FRAMEBUFFER);
                    this._renderPass = null;
                    this._colorTextures = [];
                    this._depthStencilTexture = null;
                }
                /**
                 * @en Get current render pass.
                 * @zh GFX 渲染过程。
                 */
                get renderPass() {
                    return this._renderPass;
                }
                /**
                 * @en Get current color views.
                 * @zh 颜色纹理视图数组。
                 */
                get colorTextures() {
                    return this._colorTextures;
                }
                /**
                 * @en Get current depth stencil views.
                 * @zh 深度模板纹理视图。
                 */
                get depthStencilTexture() {
                    return this._depthStencilTexture;
                }
            };
            exports_5("Framebuffer", Framebuffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/input-assembler", ["engine/cocos/core/utils/murmurhash2_gc", "engine/cocos/core/gfx/base/define"], function (exports_6, context_6) {
    "use strict";
    var murmurhash2_gc_2, define_5, InputAssembler;
    var __moduleName = context_6 && context_6.id;
    return {
        setters: [
            function (murmurhash2_gc_2_1) {
                murmurhash2_gc_2 = murmurhash2_gc_2_1;
            },
            function (define_5_1) {
                define_5 = define_5_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX input assembler.
             * @zh GFX 输入汇集器。
             */
            InputAssembler = class InputAssembler extends define_5.GFXObject {
                constructor() {
                    super(define_5.ObjectType.INPUT_ASSEMBLER);
                    this._attributes = [];
                    this._attributesHash = 0;
                    this._vertexBuffers = [];
                    this._indexBuffer = null;
                    this._indirectBuffer = null;
                    this._drawInfo = new define_5.DrawInfo();
                }
                /**
                 * @en Get current attributes.
                 * @zh 顶点属性数组。
                 */
                get attributes() {
                    return this._attributes;
                }
                /**
                 * @en Get current vertex buffers.
                 * @zh 顶点缓冲数组。
                 */
                get vertexBuffers() {
                    return this._vertexBuffers;
                }
                /**
                 * @en Get current index buffer.
                 * @zh 索引缓冲。
                 */
                get indexBuffer() {
                    return this._indexBuffer;
                }
                /**
                 * @en Get the indirect buffer, if present.
                 * @zh 间接绘制缓冲。
                 */
                get indirectBuffer() {
                    return this._indirectBuffer;
                }
                /**
                 * @en Get hash of current attributes.
                 * @zh 获取顶点属性数组的哈希值。
                 */
                get attributesHash() {
                    return this._attributesHash;
                }
                /**
                 * @en Get current vertex count.
                 * @zh 顶点数量。
                 */
                set vertexCount(count) {
                    this._drawInfo.vertexCount = count;
                }
                get vertexCount() {
                    return this._drawInfo.vertexCount;
                }
                /**
                 * @en Get starting vertex.
                 * @zh 起始顶点。
                 */
                set firstVertex(first) {
                    this._drawInfo.firstVertex = first;
                }
                get firstVertex() {
                    return this._drawInfo.firstVertex;
                }
                /**
                 * @en Get current index count.
                 * @zh 索引数量。
                 */
                set indexCount(count) {
                    this._drawInfo.indexCount = count;
                }
                get indexCount() {
                    return this._drawInfo.indexCount;
                }
                /**
                 * @en Get starting index.
                 * @zh 起始索引。
                 */
                set firstIndex(first) {
                    this._drawInfo.firstIndex = first;
                }
                get firstIndex() {
                    return this._drawInfo.firstIndex;
                }
                /**
                 * @en Get current vertex offset.
                 * @zh 顶点偏移量。
                 */
                set vertexOffset(offset) {
                    this._drawInfo.vertexOffset = offset;
                }
                get vertexOffset() {
                    return this._drawInfo.vertexOffset;
                }
                /**
                 * @en Get current instance count.
                 * @zh 实例数量。
                 */
                set instanceCount(count) {
                    this._drawInfo.instanceCount = count;
                }
                get instanceCount() {
                    return this._drawInfo.instanceCount;
                }
                /**
                 * @en Get starting instance.
                 * @zh 起始实例。
                 */
                set firstInstance(first) {
                    this._drawInfo.firstInstance = first;
                }
                get firstInstance() {
                    return this._drawInfo.firstInstance;
                }
                get drawInfo() {
                    return this._drawInfo;
                }
                /**
                 * @en Get the specified vertex buffer.
                 * @zh 获取顶点缓冲。
                 * @param stream The stream index of the vertex buffer.
                 */
                getVertexBuffer(stream = 0) {
                    if (stream < this._vertexBuffers.length) {
                        return this._vertexBuffers[stream];
                    }
                    else {
                        return null;
                    }
                }
                computeAttributesHash() {
                    let res = 'attrs';
                    for (let i = 0; i < this.attributes.length; ++i) {
                        const at = this.attributes[i];
                        res += `,${at.name},${at.format},${at.isNormalized},${at.stream},${at.isInstanced}`;
                    }
                    return murmurhash2_gc_2.murmurhash2_32_gc(res, 666);
                }
            };
            exports_6("InputAssembler", InputAssembler);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/pipeline-layout", ["engine/cocos/core/gfx/base/define"], function (exports_7, context_7) {
    "use strict";
    var define_6, PipelineLayout;
    var __moduleName = context_7 && context_7.id;
    return {
        setters: [
            function (define_6_1) {
                define_6 = define_6_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX pipeline layout.
             * @zh GFX 管线布局。
             */
            PipelineLayout = class PipelineLayout extends define_6.GFXObject {
                constructor() {
                    super(define_6.ObjectType.PIPELINE_LAYOUT);
                    this._setLayouts = [];
                }
                get setLayouts() {
                    return this._setLayouts;
                }
            };
            exports_7("PipelineLayout", PipelineLayout);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/shader", ["engine/cocos/core/gfx/base/define"], function (exports_8, context_8) {
    "use strict";
    var define_7, Shader;
    var __moduleName = context_8 && context_8.id;
    return {
        setters: [
            function (define_7_1) {
                define_7 = define_7_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX shader.
             * @zh GFX 着色器。
             */
            Shader = class Shader extends define_7.GFXObject {
                constructor() {
                    super(define_7.ObjectType.SHADER);
                    this._name = '';
                    this._stages = [];
                    this._attributes = [];
                    this._blocks = [];
                    this._samplers = [];
                }
                get name() {
                    return this._name;
                }
                get attributes() {
                    return this._attributes;
                }
                get blocks() {
                    return this._blocks;
                }
                get samplers() {
                    return this._samplers;
                }
            };
            exports_8("Shader", Shader);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/pipeline-state", ["engine/cocos/core/gfx/base/define"], function (exports_9, context_9) {
    "use strict";
    var define_8, RasterizerState, DepthStencilState, BlendTarget, BlendState, PipelineStateInfo, PipelineState;
    var __moduleName = context_9 && context_9.id;
    return {
        setters: [
            function (define_8_1) {
                define_8 = define_8_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX rasterizer state.
             * @zh GFX 光栅化状态。
             */
            RasterizerState = class RasterizerState {
                constructor(isDiscard = false, polygonMode = define_8.PolygonMode.FILL, shadeModel = define_8.ShadeModel.GOURAND, cullMode = define_8.CullMode.BACK, isFrontFaceCCW = true, depthBiasEnabled = false, depthBias = 0, depthBiasClamp = 0.0, depthBiasSlop = 0.0, isDepthClip = true, isMultisample = false, lineWidth = 1.0) {
                    this.isDiscard = isDiscard;
                    this.polygonMode = polygonMode;
                    this.shadeModel = shadeModel;
                    this.cullMode = cullMode;
                    this.isFrontFaceCCW = isFrontFaceCCW;
                    this.depthBiasEnabled = depthBiasEnabled;
                    this.depthBias = depthBias;
                    this.depthBiasClamp = depthBiasClamp;
                    this.depthBiasSlop = depthBiasSlop;
                    this.isDepthClip = isDepthClip;
                    this.isMultisample = isMultisample;
                    this.lineWidth = lineWidth;
                }
                get native() {
                    return this;
                }
                reset() {
                    this.isDiscard = false;
                    this.polygonMode = define_8.PolygonMode.FILL;
                    this.shadeModel = define_8.ShadeModel.GOURAND;
                    this.cullMode = define_8.CullMode.BACK;
                    this.isFrontFaceCCW = true;
                    this.depthBiasEnabled = false;
                    this.depthBias = 0;
                    this.depthBiasClamp = 0.0;
                    this.depthBiasSlop = 0.0;
                    this.isDepthClip = true;
                    this.isMultisample = false;
                    this.lineWidth = 1.0;
                }
                assign(rs) {
                    Object.assign(this, rs);
                }
                destroy() { }
            };
            exports_9("RasterizerState", RasterizerState);
            /**
             * @en GFX depth stencil state.
             * @zh GFX 深度模板状态。
             */
            DepthStencilState = class DepthStencilState {
                constructor(depthTest = true, depthWrite = true, depthFunc = define_8.ComparisonFunc.LESS, stencilTestFront = false, stencilFuncFront = define_8.ComparisonFunc.ALWAYS, stencilReadMaskFront = 0xffff, stencilWriteMaskFront = 0xffff, stencilFailOpFront = define_8.StencilOp.KEEP, stencilZFailOpFront = define_8.StencilOp.KEEP, stencilPassOpFront = define_8.StencilOp.KEEP, stencilRefFront = 1, stencilTestBack = false, stencilFuncBack = define_8.ComparisonFunc.ALWAYS, stencilReadMaskBack = 0xffff, stencilWriteMaskBack = 0xffff, stencilFailOpBack = define_8.StencilOp.KEEP, stencilZFailOpBack = define_8.StencilOp.KEEP, stencilPassOpBack = define_8.StencilOp.KEEP, stencilRefBack = 1) {
                    this.depthTest = depthTest;
                    this.depthWrite = depthWrite;
                    this.depthFunc = depthFunc;
                    this.stencilTestFront = stencilTestFront;
                    this.stencilFuncFront = stencilFuncFront;
                    this.stencilReadMaskFront = stencilReadMaskFront;
                    this.stencilWriteMaskFront = stencilWriteMaskFront;
                    this.stencilFailOpFront = stencilFailOpFront;
                    this.stencilZFailOpFront = stencilZFailOpFront;
                    this.stencilPassOpFront = stencilPassOpFront;
                    this.stencilRefFront = stencilRefFront;
                    this.stencilTestBack = stencilTestBack;
                    this.stencilFuncBack = stencilFuncBack;
                    this.stencilReadMaskBack = stencilReadMaskBack;
                    this.stencilWriteMaskBack = stencilWriteMaskBack;
                    this.stencilFailOpBack = stencilFailOpBack;
                    this.stencilZFailOpBack = stencilZFailOpBack;
                    this.stencilPassOpBack = stencilPassOpBack;
                    this.stencilRefBack = stencilRefBack;
                }
                get native() {
                    return this;
                }
                reset() {
                    this.depthTest = true;
                    this.depthWrite = true;
                    this.depthFunc = define_8.ComparisonFunc.LESS;
                    this.stencilTestFront = false;
                    this.stencilFuncFront = define_8.ComparisonFunc.ALWAYS;
                    this.stencilReadMaskFront = 0xffff;
                    this.stencilWriteMaskFront = 0xffff;
                    this.stencilFailOpFront = define_8.StencilOp.KEEP;
                    this.stencilZFailOpFront = define_8.StencilOp.KEEP;
                    this.stencilPassOpFront = define_8.StencilOp.KEEP;
                    this.stencilRefFront = 1;
                    this.stencilTestBack = false;
                    this.stencilFuncBack = define_8.ComparisonFunc.ALWAYS;
                    this.stencilReadMaskBack = 0xffff;
                    this.stencilWriteMaskBack = 0xffff;
                    this.stencilFailOpBack = define_8.StencilOp.KEEP;
                    this.stencilZFailOpBack = define_8.StencilOp.KEEP;
                    this.stencilPassOpBack = define_8.StencilOp.KEEP;
                    this.stencilRefBack = 1;
                }
                assign(dss) {
                    Object.assign(this, dss);
                }
                destroy() { }
            };
            exports_9("DepthStencilState", DepthStencilState);
            /**
             * @en GFX blend target.
             * @zh GFX 混合目标。
             */
            BlendTarget = class BlendTarget {
                constructor(blend = false, blendSrc = define_8.BlendFactor.ONE, blendDst = define_8.BlendFactor.ZERO, blendEq = define_8.BlendOp.ADD, blendSrcAlpha = define_8.BlendFactor.ONE, blendDstAlpha = define_8.BlendFactor.ZERO, blendAlphaEq = define_8.BlendOp.ADD, blendColorMask = define_8.ColorMask.ALL) {
                    this.blend = blend;
                    this.blendSrc = blendSrc;
                    this.blendDst = blendDst;
                    this.blendEq = blendEq;
                    this.blendSrcAlpha = blendSrcAlpha;
                    this.blendDstAlpha = blendDstAlpha;
                    this.blendAlphaEq = blendAlphaEq;
                    this.blendColorMask = blendColorMask;
                }
                reset() {
                    this.blend = false;
                    this.blendSrc = define_8.BlendFactor.ONE;
                    this.blendDst = define_8.BlendFactor.ZERO;
                    this.blendEq = define_8.BlendOp.ADD;
                    this.blendSrcAlpha = define_8.BlendFactor.ONE;
                    this.blendDstAlpha = define_8.BlendFactor.ZERO;
                    this.blendAlphaEq = define_8.BlendOp.ADD;
                    this.blendColorMask = define_8.ColorMask.ALL;
                }
                assign(target) {
                    Object.assign(this, target);
                }
                destroy() { }
            };
            exports_9("BlendTarget", BlendTarget);
            /**
             * @en GFX blend state.
             * @zh GFX 混合状态。
             */
            BlendState = class BlendState {
                constructor(isA2C = false, isIndepend = false, blendColor = new define_8.Color(), targets = [new BlendTarget()]) {
                    this.isA2C = isA2C;
                    this.isIndepend = isIndepend;
                    this.blendColor = blendColor;
                    this.targets = targets;
                }
                get native() {
                    return this;
                }
                /**
                 * @en Should use this function to set target, or it will not work
                 * on native platforms, as native can not support this feature,
                 * such as `blendState[i] = target;`.
                 *
                 * @param index The index to set target.
                 * @param target The target to be set.
                 */
                setTarget(index, target) {
                    let tg = this.targets[index];
                    if (!tg) {
                        tg = this.targets[index] = new BlendTarget();
                    }
                    Object.assign(tg, target);
                }
                reset() {
                    this.isA2C = false;
                    this.isIndepend = false;
                    this.blendColor.x = 0;
                    this.blendColor.y = 0;
                    this.blendColor.z = 0;
                    this.blendColor.w = 0;
                    this.targets.length = 1;
                    this.targets[0].reset();
                }
                destroy() { }
            };
            exports_9("BlendState", BlendState);
            PipelineStateInfo = class PipelineStateInfo {
                constructor(shader = null, pipelineLayout = null, renderPass = null, inputState = new define_8.InputState(), rasterizerState = new RasterizerState(), depthStencilState = new DepthStencilState(), blendState = new BlendState(), primitive = define_8.PrimitiveMode.TRIANGLE_LIST, dynamicStates = define_8.DynamicStateFlagBit.NONE, bindPoint = define_8.PipelineBindPoint.GRAPHICS) {
                    this.shader = shader;
                    this.pipelineLayout = pipelineLayout;
                    this.renderPass = renderPass;
                    this.inputState = inputState;
                    this.rasterizerState = rasterizerState;
                    this.depthStencilState = depthStencilState;
                    this.blendState = blendState;
                    this.primitive = primitive;
                    this.dynamicStates = dynamicStates;
                    this.bindPoint = bindPoint;
                }
            };
            exports_9("PipelineStateInfo", PipelineStateInfo);
            /**
             * @en GFX pipeline state.
             * @zh GFX 管线状态。
             */
            PipelineState = class PipelineState extends define_8.GFXObject {
                constructor() {
                    super(define_8.ObjectType.PIPELINE_STATE);
                    this._shader = null;
                    this._pipelineLayout = null;
                    this._primitive = define_8.PrimitiveMode.TRIANGLE_LIST;
                    this._is = null;
                    this._rs = new RasterizerState();
                    this._dss = new DepthStencilState();
                    this._bs = new BlendState();
                    this._dynamicStates = define_8.DynamicStateFlagBit.NONE;
                    this._renderPass = null;
                }
                /**
                 * @en Get current shader.
                 * @zh GFX 着色器。
                 */
                get shader() {
                    return this._shader;
                }
                /**
                 * @en Get current pipeline layout.
                 * @zh GFX 管线布局。
                 */
                get pipelineLayout() {
                    return this._pipelineLayout;
                }
                /**
                 * @en Get current primitve mode.
                 * @zh GFX 图元模式。
                 */
                get primitive() {
                    return this._primitive;
                }
                /**
                 * @en Get current rasterizer state.
                 * @zh GFX 光栅化状态。
                 */
                get rasterizerState() {
                    return this._rs;
                }
                /**
                 * @en Get current depth stencil state.
                 * @zh GFX 深度模板状态。
                 */
                get depthStencilState() {
                    return this._dss;
                }
                /**
                 * @en Get current blend state.
                 * @zh GFX 混合状态。
                 */
                get blendState() {
                    return this._bs;
                }
                /**
                 * @en Get current input state.
                 * @zh GFX 输入状态。
                 */
                get inputState() {
                    return this._is;
                }
                /**
                 * @en Get current dynamic states.
                 * @zh GFX 动态状态数组。
                 */
                get dynamicStates() {
                    return this._dynamicStates;
                }
                /**
                 * @en Get current render pass.
                 * @zh GFX 渲染过程。
                 */
                get renderPass() {
                    return this._renderPass;
                }
            };
            exports_9("PipelineState", PipelineState);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/states/global-barrier", ["engine/cocos/core/utils/murmurhash2_gc", "engine/cocos/core/gfx/base/define"], function (exports_10, context_10) {
    "use strict";
    var murmurhash2_gc_3, define_9, GlobalBarrier;
    var __moduleName = context_10 && context_10.id;
    return {
        setters: [
            function (murmurhash2_gc_3_1) {
                murmurhash2_gc_3 = murmurhash2_gc_3_1;
            },
            function (define_9_1) {
                define_9 = define_9_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX global barrier.
             * @zh GFX 全局内存屏障。
             */
            GlobalBarrier = class GlobalBarrier extends define_9.GFXObject {
                constructor(info, hash) {
                    super(define_9.ObjectType.GLOBAL_BARRIER);
                    this._info = new define_9.GlobalBarrierInfo();
                    this._hash = 0;
                    this._info.copy(info);
                    this._hash = hash;
                }
                get info() { return this._info; }
                get hash() { return this._hash; }
                static computeHash(info) {
                    let res = 'prev:';
                    for (let i = 0; i < info.prevAccesses.length; ++i) {
                        res += ` ${info.prevAccesses[i]}`;
                    }
                    res += 'next:';
                    for (let i = 0; i < info.nextAccesses.length; ++i) {
                        res += ` ${info.nextAccesses[i]}`;
                    }
                    return murmurhash2_gc_3.murmurhash2_32_gc(res, 666);
                }
            };
            exports_10("GlobalBarrier", GlobalBarrier);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/states/texture-barrier", ["engine/cocos/core/utils/murmurhash2_gc", "engine/cocos/core/gfx/base/define"], function (exports_11, context_11) {
    "use strict";
    var murmurhash2_gc_4, define_10, TextureBarrier;
    var __moduleName = context_11 && context_11.id;
    return {
        setters: [
            function (murmurhash2_gc_4_1) {
                murmurhash2_gc_4 = murmurhash2_gc_4_1;
            },
            function (define_10_1) {
                define_10 = define_10_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX texture barrier.
             * @zh GFX 贴图内存屏障。
             */
            TextureBarrier = class TextureBarrier extends define_10.GFXObject {
                constructor(info, hash) {
                    super(define_10.ObjectType.TEXTURE_BARRIER);
                    this._info = new define_10.TextureBarrierInfo();
                    this._hash = 0;
                    this._info.copy(info);
                    this._hash = hash;
                }
                get info() { return this._info; }
                get hash() { return this._hash; }
                static computeHash(info) {
                    let res = 'prev:';
                    for (let i = 0; i < info.prevAccesses.length; ++i) {
                        res += ` ${info.prevAccesses[i]}`;
                    }
                    res += 'next:';
                    for (let i = 0; i < info.nextAccesses.length; ++i) {
                        res += ` ${info.nextAccesses[i]}`;
                    }
                    res += info.discardContents;
                    res += info.srcQueue ? info.srcQueue.type : 0;
                    res += info.dstQueue ? info.dstQueue.type : 0;
                    return murmurhash2_gc_4.murmurhash2_32_gc(res, 666);
                }
            };
            exports_11("TextureBarrier", TextureBarrier);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/command-buffer", ["engine/cocos/core/gfx/base/define"], function (exports_12, context_12) {
    "use strict";
    var define_11, CommandBuffer;
    var __moduleName = context_12 && context_12.id;
    return {
        setters: [
            function (define_11_1) {
                define_11 = define_11_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX command buffer.
             * @zh GFX 命令缓冲。
             */
            CommandBuffer = class CommandBuffer extends define_11.GFXObject {
                constructor() {
                    super(define_11.ObjectType.COMMAND_BUFFER);
                    this._queue = null;
                    this._type = define_11.CommandBufferType.PRIMARY;
                    this._numDrawCalls = 0;
                    this._numInstances = 0;
                    this._numTris = 0;
                }
                /**
                 * @en Type of the command buffer.
                 * @zh 命令缓冲类型。
                 */
                get type() {
                    return this._type;
                }
                /**
                 * @en Type of the command buffer.
                 * @zh 命令缓冲类型。
                 */
                get queue() {
                    return this._queue;
                }
                /**
                 * @en Number of draw calls currently recorded.
                 * @zh 绘制调用次数。
                 */
                get numDrawCalls() {
                    return this._numDrawCalls;
                }
                /**
                 * @en Number of instances currently recorded.
                 * @zh 绘制 Instance 数量。
                 */
                get numInstances() {
                    return this._numInstances;
                }
                /**
                 * @en Number of triangles currently recorded.
                 * @zh 绘制三角形数量。
                 */
                get numTris() {
                    return this._numTris;
                }
            };
            exports_12("CommandBuffer", CommandBuffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/queue", ["engine/cocos/core/gfx/base/define"], function (exports_13, context_13) {
    "use strict";
    var define_12, Queue;
    var __moduleName = context_13 && context_13.id;
    return {
        setters: [
            function (define_12_1) {
                define_12 = define_12_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX Queue.
             * @zh GFX 队列。
             */
            Queue = class Queue extends define_12.GFXObject {
                constructor() {
                    super(define_12.ObjectType.QUEUE);
                    this._type = define_12.QueueType.GRAPHICS;
                }
                /**
                 * @en Get current type.
                 * @zh 队列类型。
                 */
                get type() {
                    return this._type;
                }
            };
            exports_13("Queue", Queue);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/states/sampler", ["engine/cocos/core/gfx/base/define"], function (exports_14, context_14) {
    "use strict";
    var define_13, Sampler;
    var __moduleName = context_14 && context_14.id;
    return {
        setters: [
            function (define_13_1) {
                define_13 = define_13_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX sampler.
             * @zh GFX 采样器。
             */
            Sampler = class Sampler extends define_13.GFXObject {
                constructor(info, hash) {
                    super(define_13.ObjectType.SAMPLER);
                    this._info = new define_13.SamplerInfo();
                    this._hash = 0;
                    this._info.copy(info);
                    this._hash = hash;
                }
                get info() { return this._info; }
                get hash() { return this._hash; }
                static computeHash(info) {
                    let hash = info.minFilter;
                    hash |= (info.magFilter << 2);
                    hash |= (info.mipFilter << 4);
                    hash |= (info.addressU << 6);
                    hash |= (info.addressV << 8);
                    hash |= (info.addressW << 10);
                    hash |= (info.maxAnisotropy << 12);
                    hash |= (info.cmpFunc << 16);
                    return hash;
                }
                static unpackFromHash(hash) {
                    const info = new define_13.SamplerInfo();
                    info.minFilter = (hash & ((1 << 2) - 1)) >> 0;
                    info.magFilter = (hash & ((1 << 2) - 1)) >> 2;
                    info.mipFilter = (hash & ((1 << 2) - 1)) >> 4;
                    info.addressU = (hash & ((1 << 2) - 1)) >> 6;
                    info.addressV = (hash & ((1 << 2) - 1)) >> 8;
                    info.addressW = (hash & ((1 << 2) - 1)) >> 10;
                    info.maxAnisotropy = (hash & ((1 << 4) - 1)) >> 12;
                    info.cmpFunc = (hash & ((1 << 3) - 1)) >> 16;
                    return info;
                }
            };
            exports_14("Sampler", Sampler);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/swapchain", ["engine/cocos/core/gfx/base/define"], function (exports_15, context_15) {
    "use strict";
    var define_14, Swapchain;
    var __moduleName = context_15 && context_15.id;
    return {
        setters: [
            function (define_14_1) {
                define_14 = define_14_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX Swapchain.
             * @zh GFX 交换链。
             */
            Swapchain = class Swapchain extends define_14.GFXObject {
                constructor() {
                    super(define_14.ObjectType.SWAPCHAIN);
                    this._transform = define_14.SurfaceTransform.IDENTITY;
                    this._colorTexture = null;
                    this._depthStencilTexture = null;
                }
                /**
                 * @en The color texture of this swapchain.
                 * @zh 当前交换链的颜色缓冲。
                 */
                get colorTexture() {
                    return this._colorTexture;
                }
                /**
                 * @en The depth stencil texture of this swapchain.
                 * @zh 当前交换链的深度模板缓冲。
                 */
                get depthStencilTexture() {
                    return this._depthStencilTexture;
                }
                /**
                 * @en The surface transform to be applied in projection matrices.
                 * @zh 需要在投影矩阵中应用的表面变换。
                 */
                get surfaceTransform() {
                    return this._transform;
                }
                get width() {
                    return this._colorTexture.width;
                }
                get height() {
                    return this._colorTexture.height;
                }
            };
            exports_15("Swapchain", Swapchain);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/define", [], function (exports_16, context_16) {
    "use strict";
    var deepCopy, ObjectType, Status, API, SurfaceTransform, Feature, Format, FormatType, Type, BufferUsageBit, BufferFlagBit, MemoryAccessBit, MemoryUsageBit, TextureType, TextureUsageBit, TextureFlagBit, SampleCount, VsyncMode, Filter, Address, ComparisonFunc, StencilOp, BlendFactor, BlendOp, ColorMask, ShaderStageFlagBit, LoadOp, StoreOp, AccessType, ResolveMode, PipelineBindPoint, PrimitiveMode, PolygonMode, ShadeModel, CullMode, DynamicStateFlagBit, StencilFace, DescriptorType, QueueType, CommandBufferType, ClearFlagBit, Size, DeviceCaps, Offset, Rect, Extent, TextureSubresLayers, TextureSubresRange, TextureCopy, TextureBlit, BufferTextureCopy, Viewport, Color, BindingMappingInfo, SwapchainInfo, DeviceInfo, BufferInfo, BufferViewInfo, DrawInfo, DispatchInfo, IndirectBuffer, TextureInfo, TextureViewInfo, SamplerInfo, Uniform, UniformBlock, UniformSamplerTexture, UniformSampler, UniformTexture, UniformStorageImage, UniformStorageBuffer, UniformInputAttachment, ShaderStage, Attribute, ShaderInfo, InputAssemblerInfo, ColorAttachment, DepthStencilAttachment, SubpassInfo, SubpassDependency, RenderPassInfo, GlobalBarrierInfo, TextureBarrierInfo, FramebufferInfo, DescriptorSetLayoutBinding, DescriptorSetLayoutInfo, DescriptorSetInfo, PipelineLayoutInfo, InputState, CommandBufferInfo, QueueInfo, FormatInfo, MemoryStatus, DynamicStencilStates, DynamicStates, GFXObject, AttributeName, FormatInfos, DESCRIPTOR_BUFFER_TYPE, DESCRIPTOR_SAMPLER_TYPE, DESCRIPTOR_DYNAMIC_TYPE, DRAW_INFO_SIZE, _type2size;
    var __moduleName = context_16 && context_16.id;
    function IsPowerOf2(x) {
        return x > 0 && (x & (x - 1)) === 0;
    }
    exports_16("IsPowerOf2", IsPowerOf2);
    /**
     * @en Get memory size of the specified fomat.
     * @zh 获取指定格式对应的内存大小。
     * @param format The target format.
     * @param width The target width.
     * @param height The target height.
     * @param depth The target depth.
     */
    function FormatSize(format, width, height, depth) {
        if (!FormatInfos[format].isCompressed) {
            return (width * height * depth * FormatInfos[format].size);
        }
        else {
            switch (format) {
                case Format.BC1:
                case Format.BC1_ALPHA:
                case Format.BC1_SRGB:
                case Format.BC1_SRGB_ALPHA:
                    return Math.ceil(width / 4) * Math.ceil(height / 4) * 8 * depth;
                case Format.BC2:
                case Format.BC2_SRGB:
                case Format.BC3:
                case Format.BC3_SRGB:
                case Format.BC4:
                case Format.BC4_SNORM:
                case Format.BC6H_SF16:
                case Format.BC6H_UF16:
                case Format.BC7:
                case Format.BC7_SRGB:
                    return Math.ceil(width / 4) * Math.ceil(height / 4) * 16 * depth;
                case Format.BC5:
                case Format.BC5_SNORM:
                    return Math.ceil(width / 4) * Math.ceil(height / 4) * 32 * depth;
                case Format.ETC_RGB8:
                case Format.ETC2_RGB8:
                case Format.ETC2_SRGB8:
                case Format.ETC2_RGB8_A1:
                case Format.EAC_R11:
                case Format.EAC_R11SN:
                    return Math.ceil(width / 4) * Math.ceil(height / 4) * 8 * depth;
                case Format.ETC2_RGBA8:
                case Format.ETC2_SRGB8_A1:
                case Format.EAC_RG11:
                case Format.EAC_RG11SN:
                    return Math.ceil(width / 4) * Math.ceil(height / 4) * 16 * depth;
                case Format.PVRTC_RGB2:
                case Format.PVRTC_RGBA2:
                case Format.PVRTC2_2BPP:
                    return Math.ceil(Math.max(width, 16) * Math.max(height, 8) / 4) * depth;
                case Format.PVRTC_RGB4:
                case Format.PVRTC_RGBA4:
                case Format.PVRTC2_4BPP:
                    return Math.ceil(Math.max(width, 8) * Math.max(height, 8) / 2) * depth;
                case Format.ASTC_RGBA_4X4:
                case Format.ASTC_SRGBA_4X4:
                    return Math.ceil(width / 4) * Math.ceil(height / 4) * 16 * depth;
                case Format.ASTC_RGBA_5X4:
                case Format.ASTC_SRGBA_5X4:
                    return Math.ceil(width / 5) * Math.ceil(height / 4) * 16 * depth;
                case Format.ASTC_RGBA_5X5:
                case Format.ASTC_SRGBA_5X5:
                    return Math.ceil(width / 5) * Math.ceil(height / 5) * 16 * depth;
                case Format.ASTC_RGBA_6X5:
                case Format.ASTC_SRGBA_6X5:
                    return Math.ceil(width / 6) * Math.ceil(height / 5) * 16 * depth;
                case Format.ASTC_RGBA_6X6:
                case Format.ASTC_SRGBA_6X6:
                    return Math.ceil(width / 6) * Math.ceil(height / 6) * 16 * depth;
                case Format.ASTC_RGBA_8X5:
                case Format.ASTC_SRGBA_8X5:
                    return Math.ceil(width / 8) * Math.ceil(height / 5) * 16 * depth;
                case Format.ASTC_RGBA_8X6:
                case Format.ASTC_SRGBA_8X6:
                    return Math.ceil(width / 8) * Math.ceil(height / 6) * 16 * depth;
                case Format.ASTC_RGBA_8X8:
                case Format.ASTC_SRGBA_8X8:
                    return Math.ceil(width / 8) * Math.ceil(height / 8) * 16 * depth;
                case Format.ASTC_RGBA_10X5:
                case Format.ASTC_SRGBA_10X5:
                    return Math.ceil(width / 10) * Math.ceil(height / 5) * 16 * depth;
                case Format.ASTC_RGBA_10X6:
                case Format.ASTC_SRGBA_10X6:
                    return Math.ceil(width / 10) * Math.ceil(height / 6) * 16 * depth;
                case Format.ASTC_RGBA_10X8:
                case Format.ASTC_SRGBA_10X8:
                    return Math.ceil(width / 10) * Math.ceil(height / 8) * 16 * depth;
                case Format.ASTC_RGBA_10X10:
                case Format.ASTC_SRGBA_10X10:
                    return Math.ceil(width / 10) * Math.ceil(height / 10) * 16 * depth;
                case Format.ASTC_RGBA_12X10:
                case Format.ASTC_SRGBA_12X10:
                    return Math.ceil(width / 12) * Math.ceil(height / 10) * 16 * depth;
                case Format.ASTC_RGBA_12X12:
                case Format.ASTC_SRGBA_12X12:
                    return Math.ceil(width / 12) * Math.ceil(height / 12) * 16 * depth;
                default: {
                    return 0;
                }
            }
        }
    }
    exports_16("FormatSize", FormatSize);
    /**
     * @en Get memory size of the specified surface.
     * @zh GFX 格式表面内存大小。
     * @param format The target format.
     * @param width The target width.
     * @param height The target height.
     * @param depth The target depth.
     * @param mips The target mip levels.
     */
    function FormatSurfaceSize(format, width, height, depth, mips) {
        let size = 0;
        for (let i = 0; i < mips; ++i) {
            size += FormatSize(format, width, height, depth);
            width = Math.max(width >> 1, 1);
            height = Math.max(height >> 1, 1);
        }
        return size;
    }
    exports_16("FormatSurfaceSize", FormatSurfaceSize);
    /**
     * @en Get the memory size of the specified type.
     * @zh 得到 GFX 数据类型的大小。
     * @param type The target type.
     */
    function GetTypeSize(type) {
        return _type2size[type] || 0;
    }
    exports_16("GetTypeSize", GetTypeSize);
    function getTypedArrayConstructor(info) {
        const stride = info.size / info.count;
        switch (info.type) {
            case FormatType.UNORM:
            case FormatType.UINT: {
                switch (stride) {
                    case 1: return Uint8Array;
                    case 2: return Uint16Array;
                    case 4: return Uint32Array;
                    default:
                }
                break;
            }
            case FormatType.SNORM:
            case FormatType.INT: {
                switch (stride) {
                    case 1: return Int8Array;
                    case 2: return Int16Array;
                    case 4: return Int32Array;
                    default:
                }
                break;
            }
            case FormatType.FLOAT: {
                return Float32Array;
            }
            default:
        }
        return Float32Array;
    }
    exports_16("getTypedArrayConstructor", getTypedArrayConstructor);
    return {
        setters: [],
        execute: function () {
            deepCopy = (target, source, Ctor) => {
                for (let i = 0; i < source.length; ++i) {
                    if (target.length <= i)
                        target.push(new Ctor());
                    target[i].copy(source[i]);
                }
                target.length = source.length;
            };
            /**
             * ========================= !DO NOT CHANGE THE FOLLOWING SECTION MANUALLY! =========================
             * The following section is auto-generated from engine-native/cocos/renderer/core/gfx/GFXDef-common.h
             * by the script engine-native/tools/gfx-define-generator/generate.js.
             * Changes to these public interfaces should be made there first and synced back.
             * ========================= !DO NOT CHANGE THE FOLLOWING SECTION MANUALLY! =========================
             */
            (function (ObjectType) {
                ObjectType[ObjectType["UNKNOWN"] = 0] = "UNKNOWN";
                ObjectType[ObjectType["SWAPCHAIN"] = 1] = "SWAPCHAIN";
                ObjectType[ObjectType["BUFFER"] = 2] = "BUFFER";
                ObjectType[ObjectType["TEXTURE"] = 3] = "TEXTURE";
                ObjectType[ObjectType["RENDER_PASS"] = 4] = "RENDER_PASS";
                ObjectType[ObjectType["FRAMEBUFFER"] = 5] = "FRAMEBUFFER";
                ObjectType[ObjectType["SAMPLER"] = 6] = "SAMPLER";
                ObjectType[ObjectType["SHADER"] = 7] = "SHADER";
                ObjectType[ObjectType["DESCRIPTOR_SET_LAYOUT"] = 8] = "DESCRIPTOR_SET_LAYOUT";
                ObjectType[ObjectType["PIPELINE_LAYOUT"] = 9] = "PIPELINE_LAYOUT";
                ObjectType[ObjectType["PIPELINE_STATE"] = 10] = "PIPELINE_STATE";
                ObjectType[ObjectType["DESCRIPTOR_SET"] = 11] = "DESCRIPTOR_SET";
                ObjectType[ObjectType["INPUT_ASSEMBLER"] = 12] = "INPUT_ASSEMBLER";
                ObjectType[ObjectType["COMMAND_BUFFER"] = 13] = "COMMAND_BUFFER";
                ObjectType[ObjectType["QUEUE"] = 14] = "QUEUE";
                ObjectType[ObjectType["GLOBAL_BARRIER"] = 15] = "GLOBAL_BARRIER";
                ObjectType[ObjectType["TEXTURE_BARRIER"] = 16] = "TEXTURE_BARRIER";
                ObjectType[ObjectType["BUFFER_BARRIER"] = 17] = "BUFFER_BARRIER";
                ObjectType[ObjectType["COUNT"] = 18] = "COUNT";
            })(ObjectType || (ObjectType = {}));
            exports_16("ObjectType", ObjectType);
            (function (Status) {
                Status[Status["UNREADY"] = 0] = "UNREADY";
                Status[Status["FAILED"] = 1] = "FAILED";
                Status[Status["SUCCESS"] = 2] = "SUCCESS";
            })(Status || (Status = {}));
            exports_16("Status", Status);
            (function (API) {
                API[API["UNKNOWN"] = 0] = "UNKNOWN";
                API[API["GLES2"] = 1] = "GLES2";
                API[API["GLES3"] = 2] = "GLES3";
                API[API["METAL"] = 3] = "METAL";
                API[API["VULKAN"] = 4] = "VULKAN";
                API[API["WEBGL"] = 5] = "WEBGL";
                API[API["WEBGL2"] = 6] = "WEBGL2";
                API[API["WEBGPU"] = 7] = "WEBGPU";
            })(API || (API = {}));
            exports_16("API", API);
            (function (SurfaceTransform) {
                SurfaceTransform[SurfaceTransform["IDENTITY"] = 0] = "IDENTITY";
                SurfaceTransform[SurfaceTransform["ROTATE_90"] = 1] = "ROTATE_90";
                SurfaceTransform[SurfaceTransform["ROTATE_180"] = 2] = "ROTATE_180";
                SurfaceTransform[SurfaceTransform["ROTATE_270"] = 3] = "ROTATE_270";
            })(SurfaceTransform || (SurfaceTransform = {}));
            exports_16("SurfaceTransform", SurfaceTransform);
            (function (Feature) {
                Feature[Feature["COLOR_FLOAT"] = 0] = "COLOR_FLOAT";
                Feature[Feature["COLOR_HALF_FLOAT"] = 1] = "COLOR_HALF_FLOAT";
                Feature[Feature["TEXTURE_FLOAT"] = 2] = "TEXTURE_FLOAT";
                Feature[Feature["TEXTURE_HALF_FLOAT"] = 3] = "TEXTURE_HALF_FLOAT";
                Feature[Feature["TEXTURE_FLOAT_LINEAR"] = 4] = "TEXTURE_FLOAT_LINEAR";
                Feature[Feature["TEXTURE_HALF_FLOAT_LINEAR"] = 5] = "TEXTURE_HALF_FLOAT_LINEAR";
                Feature[Feature["FORMAT_R11G11B10F"] = 6] = "FORMAT_R11G11B10F";
                Feature[Feature["FORMAT_SRGB"] = 7] = "FORMAT_SRGB";
                Feature[Feature["FORMAT_ETC1"] = 8] = "FORMAT_ETC1";
                Feature[Feature["FORMAT_ETC2"] = 9] = "FORMAT_ETC2";
                Feature[Feature["FORMAT_DXT"] = 10] = "FORMAT_DXT";
                Feature[Feature["FORMAT_PVRTC"] = 11] = "FORMAT_PVRTC";
                Feature[Feature["FORMAT_ASTC"] = 12] = "FORMAT_ASTC";
                Feature[Feature["FORMAT_RGB8"] = 13] = "FORMAT_RGB8";
                Feature[Feature["ELEMENT_INDEX_UINT"] = 14] = "ELEMENT_INDEX_UINT";
                Feature[Feature["INSTANCED_ARRAYS"] = 15] = "INSTANCED_ARRAYS";
                Feature[Feature["MULTIPLE_RENDER_TARGETS"] = 16] = "MULTIPLE_RENDER_TARGETS";
                Feature[Feature["BLEND_MINMAX"] = 17] = "BLEND_MINMAX";
                Feature[Feature["COMPUTE_SHADER"] = 18] = "COMPUTE_SHADER";
                // This flag indicates whether the device can benefit from subpass-style usages.
                // Specifically, this only differs on the GLES backends: the Framebuffer Fetch
                // extension is used to simulate input attachments, so the flag is not set when
                // the extension is not supported, and you should switch to the fallback branch
                // (without the extension requirement) in GLSL shader sources accordingly.
                // Everything else can remain the same.
                //
                // Another caveat when using the Framebuffer Fetch extensions in shaders is that
                // for subpasses with exactly 4 inout attachments the output is automatically set
                // to the last attachment (taking advantage of 'inout' property), and a separate
                // blit operation (if needed) will be added for you afterwards to transfer the
                // rendering result to the corrent subpass output texture. This is to ameliorate
                // the max number of attachment limit(4) situation for many devices, and shader
                // sources inside this kind of subpass must match this behavior.
                Feature[Feature["INPUT_ATTACHMENT_BENEFIT"] = 19] = "INPUT_ATTACHMENT_BENEFIT";
                Feature[Feature["COUNT"] = 20] = "COUNT";
            })(Feature || (Feature = {}));
            exports_16("Feature", Feature);
            (function (Format) {
                Format[Format["UNKNOWN"] = 0] = "UNKNOWN";
                Format[Format["A8"] = 1] = "A8";
                Format[Format["L8"] = 2] = "L8";
                Format[Format["LA8"] = 3] = "LA8";
                Format[Format["R8"] = 4] = "R8";
                Format[Format["R8SN"] = 5] = "R8SN";
                Format[Format["R8UI"] = 6] = "R8UI";
                Format[Format["R8I"] = 7] = "R8I";
                Format[Format["R16F"] = 8] = "R16F";
                Format[Format["R16UI"] = 9] = "R16UI";
                Format[Format["R16I"] = 10] = "R16I";
                Format[Format["R32F"] = 11] = "R32F";
                Format[Format["R32UI"] = 12] = "R32UI";
                Format[Format["R32I"] = 13] = "R32I";
                Format[Format["RG8"] = 14] = "RG8";
                Format[Format["RG8SN"] = 15] = "RG8SN";
                Format[Format["RG8UI"] = 16] = "RG8UI";
                Format[Format["RG8I"] = 17] = "RG8I";
                Format[Format["RG16F"] = 18] = "RG16F";
                Format[Format["RG16UI"] = 19] = "RG16UI";
                Format[Format["RG16I"] = 20] = "RG16I";
                Format[Format["RG32F"] = 21] = "RG32F";
                Format[Format["RG32UI"] = 22] = "RG32UI";
                Format[Format["RG32I"] = 23] = "RG32I";
                Format[Format["RGB8"] = 24] = "RGB8";
                Format[Format["SRGB8"] = 25] = "SRGB8";
                Format[Format["RGB8SN"] = 26] = "RGB8SN";
                Format[Format["RGB8UI"] = 27] = "RGB8UI";
                Format[Format["RGB8I"] = 28] = "RGB8I";
                Format[Format["RGB16F"] = 29] = "RGB16F";
                Format[Format["RGB16UI"] = 30] = "RGB16UI";
                Format[Format["RGB16I"] = 31] = "RGB16I";
                Format[Format["RGB32F"] = 32] = "RGB32F";
                Format[Format["RGB32UI"] = 33] = "RGB32UI";
                Format[Format["RGB32I"] = 34] = "RGB32I";
                Format[Format["RGBA8"] = 35] = "RGBA8";
                Format[Format["BGRA8"] = 36] = "BGRA8";
                Format[Format["SRGB8_A8"] = 37] = "SRGB8_A8";
                Format[Format["RGBA8SN"] = 38] = "RGBA8SN";
                Format[Format["RGBA8UI"] = 39] = "RGBA8UI";
                Format[Format["RGBA8I"] = 40] = "RGBA8I";
                Format[Format["RGBA16F"] = 41] = "RGBA16F";
                Format[Format["RGBA16UI"] = 42] = "RGBA16UI";
                Format[Format["RGBA16I"] = 43] = "RGBA16I";
                Format[Format["RGBA32F"] = 44] = "RGBA32F";
                Format[Format["RGBA32UI"] = 45] = "RGBA32UI";
                Format[Format["RGBA32I"] = 46] = "RGBA32I";
                // Special Format
                Format[Format["R5G6B5"] = 47] = "R5G6B5";
                Format[Format["R11G11B10F"] = 48] = "R11G11B10F";
                Format[Format["RGB5A1"] = 49] = "RGB5A1";
                Format[Format["RGBA4"] = 50] = "RGBA4";
                Format[Format["RGB10A2"] = 51] = "RGB10A2";
                Format[Format["RGB10A2UI"] = 52] = "RGB10A2UI";
                Format[Format["RGB9E5"] = 53] = "RGB9E5";
                // Depth-Stencil Format
                Format[Format["DEPTH"] = 54] = "DEPTH";
                Format[Format["DEPTH_STENCIL"] = 55] = "DEPTH_STENCIL";
                // Compressed Format
                // Block Compression Format, DDS (DirectDraw Surface)
                // DXT1: 3 channels (5:6:5), 1/8 origianl size, with 0 or 1 bit of alpha
                Format[Format["BC1"] = 56] = "BC1";
                Format[Format["BC1_ALPHA"] = 57] = "BC1_ALPHA";
                Format[Format["BC1_SRGB"] = 58] = "BC1_SRGB";
                Format[Format["BC1_SRGB_ALPHA"] = 59] = "BC1_SRGB_ALPHA";
                // DXT3: 4 channels (5:6:5), 1/4 origianl size, with 4 bits of alpha
                Format[Format["BC2"] = 60] = "BC2";
                Format[Format["BC2_SRGB"] = 61] = "BC2_SRGB";
                // DXT5: 4 channels (5:6:5), 1/4 origianl size, with 8 bits of alpha
                Format[Format["BC3"] = 62] = "BC3";
                Format[Format["BC3_SRGB"] = 63] = "BC3_SRGB";
                // 1 channel (8), 1/4 origianl size
                Format[Format["BC4"] = 64] = "BC4";
                Format[Format["BC4_SNORM"] = 65] = "BC4_SNORM";
                // 2 channels (8:8), 1/2 origianl size
                Format[Format["BC5"] = 66] = "BC5";
                Format[Format["BC5_SNORM"] = 67] = "BC5_SNORM";
                // 3 channels (16:16:16), half-floating point, 1/6 origianl size
                // UF16: unsigned float, 5 exponent bits + 11 mantissa bits
                // SF16: signed float, 1 signed bit + 5 exponent bits + 10 mantissa bits
                Format[Format["BC6H_UF16"] = 68] = "BC6H_UF16";
                Format[Format["BC6H_SF16"] = 69] = "BC6H_SF16";
                // 4 channels (4~7 bits per channel) with 0 to 8 bits of alpha, 1/3 original size
                Format[Format["BC7"] = 70] = "BC7";
                Format[Format["BC7_SRGB"] = 71] = "BC7_SRGB";
                // Ericsson Texture Compression Format
                Format[Format["ETC_RGB8"] = 72] = "ETC_RGB8";
                Format[Format["ETC2_RGB8"] = 73] = "ETC2_RGB8";
                Format[Format["ETC2_SRGB8"] = 74] = "ETC2_SRGB8";
                Format[Format["ETC2_RGB8_A1"] = 75] = "ETC2_RGB8_A1";
                Format[Format["ETC2_SRGB8_A1"] = 76] = "ETC2_SRGB8_A1";
                Format[Format["ETC2_RGBA8"] = 77] = "ETC2_RGBA8";
                Format[Format["ETC2_SRGB8_A8"] = 78] = "ETC2_SRGB8_A8";
                Format[Format["EAC_R11"] = 79] = "EAC_R11";
                Format[Format["EAC_R11SN"] = 80] = "EAC_R11SN";
                Format[Format["EAC_RG11"] = 81] = "EAC_RG11";
                Format[Format["EAC_RG11SN"] = 82] = "EAC_RG11SN";
                // PVRTC (PowerVR)
                Format[Format["PVRTC_RGB2"] = 83] = "PVRTC_RGB2";
                Format[Format["PVRTC_RGBA2"] = 84] = "PVRTC_RGBA2";
                Format[Format["PVRTC_RGB4"] = 85] = "PVRTC_RGB4";
                Format[Format["PVRTC_RGBA4"] = 86] = "PVRTC_RGBA4";
                Format[Format["PVRTC2_2BPP"] = 87] = "PVRTC2_2BPP";
                Format[Format["PVRTC2_4BPP"] = 88] = "PVRTC2_4BPP";
                // ASTC (Adaptive Scalable Texture Compression)
                Format[Format["ASTC_RGBA_4X4"] = 89] = "ASTC_RGBA_4X4";
                Format[Format["ASTC_RGBA_5X4"] = 90] = "ASTC_RGBA_5X4";
                Format[Format["ASTC_RGBA_5X5"] = 91] = "ASTC_RGBA_5X5";
                Format[Format["ASTC_RGBA_6X5"] = 92] = "ASTC_RGBA_6X5";
                Format[Format["ASTC_RGBA_6X6"] = 93] = "ASTC_RGBA_6X6";
                Format[Format["ASTC_RGBA_8X5"] = 94] = "ASTC_RGBA_8X5";
                Format[Format["ASTC_RGBA_8X6"] = 95] = "ASTC_RGBA_8X6";
                Format[Format["ASTC_RGBA_8X8"] = 96] = "ASTC_RGBA_8X8";
                Format[Format["ASTC_RGBA_10X5"] = 97] = "ASTC_RGBA_10X5";
                Format[Format["ASTC_RGBA_10X6"] = 98] = "ASTC_RGBA_10X6";
                Format[Format["ASTC_RGBA_10X8"] = 99] = "ASTC_RGBA_10X8";
                Format[Format["ASTC_RGBA_10X10"] = 100] = "ASTC_RGBA_10X10";
                Format[Format["ASTC_RGBA_12X10"] = 101] = "ASTC_RGBA_12X10";
                Format[Format["ASTC_RGBA_12X12"] = 102] = "ASTC_RGBA_12X12";
                // ASTC (Adaptive Scalable Texture Compression) SRGB
                Format[Format["ASTC_SRGBA_4X4"] = 103] = "ASTC_SRGBA_4X4";
                Format[Format["ASTC_SRGBA_5X4"] = 104] = "ASTC_SRGBA_5X4";
                Format[Format["ASTC_SRGBA_5X5"] = 105] = "ASTC_SRGBA_5X5";
                Format[Format["ASTC_SRGBA_6X5"] = 106] = "ASTC_SRGBA_6X5";
                Format[Format["ASTC_SRGBA_6X6"] = 107] = "ASTC_SRGBA_6X6";
                Format[Format["ASTC_SRGBA_8X5"] = 108] = "ASTC_SRGBA_8X5";
                Format[Format["ASTC_SRGBA_8X6"] = 109] = "ASTC_SRGBA_8X6";
                Format[Format["ASTC_SRGBA_8X8"] = 110] = "ASTC_SRGBA_8X8";
                Format[Format["ASTC_SRGBA_10X5"] = 111] = "ASTC_SRGBA_10X5";
                Format[Format["ASTC_SRGBA_10X6"] = 112] = "ASTC_SRGBA_10X6";
                Format[Format["ASTC_SRGBA_10X8"] = 113] = "ASTC_SRGBA_10X8";
                Format[Format["ASTC_SRGBA_10X10"] = 114] = "ASTC_SRGBA_10X10";
                Format[Format["ASTC_SRGBA_12X10"] = 115] = "ASTC_SRGBA_12X10";
                Format[Format["ASTC_SRGBA_12X12"] = 116] = "ASTC_SRGBA_12X12";
                // Total count
                Format[Format["COUNT"] = 117] = "COUNT";
            })(Format || (Format = {}));
            exports_16("Format", Format);
            (function (FormatType) {
                FormatType[FormatType["NONE"] = 0] = "NONE";
                FormatType[FormatType["UNORM"] = 1] = "UNORM";
                FormatType[FormatType["SNORM"] = 2] = "SNORM";
                FormatType[FormatType["UINT"] = 3] = "UINT";
                FormatType[FormatType["INT"] = 4] = "INT";
                FormatType[FormatType["UFLOAT"] = 5] = "UFLOAT";
                FormatType[FormatType["FLOAT"] = 6] = "FLOAT";
            })(FormatType || (FormatType = {}));
            exports_16("FormatType", FormatType);
            (function (Type) {
                Type[Type["UNKNOWN"] = 0] = "UNKNOWN";
                Type[Type["BOOL"] = 1] = "BOOL";
                Type[Type["BOOL2"] = 2] = "BOOL2";
                Type[Type["BOOL3"] = 3] = "BOOL3";
                Type[Type["BOOL4"] = 4] = "BOOL4";
                Type[Type["INT"] = 5] = "INT";
                Type[Type["INT2"] = 6] = "INT2";
                Type[Type["INT3"] = 7] = "INT3";
                Type[Type["INT4"] = 8] = "INT4";
                Type[Type["UINT"] = 9] = "UINT";
                Type[Type["UINT2"] = 10] = "UINT2";
                Type[Type["UINT3"] = 11] = "UINT3";
                Type[Type["UINT4"] = 12] = "UINT4";
                Type[Type["FLOAT"] = 13] = "FLOAT";
                Type[Type["FLOAT2"] = 14] = "FLOAT2";
                Type[Type["FLOAT3"] = 15] = "FLOAT3";
                Type[Type["FLOAT4"] = 16] = "FLOAT4";
                Type[Type["MAT2"] = 17] = "MAT2";
                Type[Type["MAT2X3"] = 18] = "MAT2X3";
                Type[Type["MAT2X4"] = 19] = "MAT2X4";
                Type[Type["MAT3X2"] = 20] = "MAT3X2";
                Type[Type["MAT3"] = 21] = "MAT3";
                Type[Type["MAT3X4"] = 22] = "MAT3X4";
                Type[Type["MAT4X2"] = 23] = "MAT4X2";
                Type[Type["MAT4X3"] = 24] = "MAT4X3";
                Type[Type["MAT4"] = 25] = "MAT4";
                // combined image samplers
                Type[Type["SAMPLER1D"] = 26] = "SAMPLER1D";
                Type[Type["SAMPLER1D_ARRAY"] = 27] = "SAMPLER1D_ARRAY";
                Type[Type["SAMPLER2D"] = 28] = "SAMPLER2D";
                Type[Type["SAMPLER2D_ARRAY"] = 29] = "SAMPLER2D_ARRAY";
                Type[Type["SAMPLER3D"] = 30] = "SAMPLER3D";
                Type[Type["SAMPLER_CUBE"] = 31] = "SAMPLER_CUBE";
                // sampler
                Type[Type["SAMPLER"] = 32] = "SAMPLER";
                // sampled textures
                Type[Type["TEXTURE1D"] = 33] = "TEXTURE1D";
                Type[Type["TEXTURE1D_ARRAY"] = 34] = "TEXTURE1D_ARRAY";
                Type[Type["TEXTURE2D"] = 35] = "TEXTURE2D";
                Type[Type["TEXTURE2D_ARRAY"] = 36] = "TEXTURE2D_ARRAY";
                Type[Type["TEXTURE3D"] = 37] = "TEXTURE3D";
                Type[Type["TEXTURE_CUBE"] = 38] = "TEXTURE_CUBE";
                // storage images
                Type[Type["IMAGE1D"] = 39] = "IMAGE1D";
                Type[Type["IMAGE1D_ARRAY"] = 40] = "IMAGE1D_ARRAY";
                Type[Type["IMAGE2D"] = 41] = "IMAGE2D";
                Type[Type["IMAGE2D_ARRAY"] = 42] = "IMAGE2D_ARRAY";
                Type[Type["IMAGE3D"] = 43] = "IMAGE3D";
                Type[Type["IMAGE_CUBE"] = 44] = "IMAGE_CUBE";
                // input attachment
                Type[Type["SUBPASS_INPUT"] = 45] = "SUBPASS_INPUT";
                Type[Type["COUNT"] = 46] = "COUNT";
            })(Type || (Type = {}));
            exports_16("Type", Type);
            (function (BufferUsageBit) {
                BufferUsageBit[BufferUsageBit["NONE"] = 0] = "NONE";
                BufferUsageBit[BufferUsageBit["TRANSFER_SRC"] = 1] = "TRANSFER_SRC";
                BufferUsageBit[BufferUsageBit["TRANSFER_DST"] = 2] = "TRANSFER_DST";
                BufferUsageBit[BufferUsageBit["INDEX"] = 4] = "INDEX";
                BufferUsageBit[BufferUsageBit["VERTEX"] = 8] = "VERTEX";
                BufferUsageBit[BufferUsageBit["UNIFORM"] = 16] = "UNIFORM";
                BufferUsageBit[BufferUsageBit["STORAGE"] = 32] = "STORAGE";
                BufferUsageBit[BufferUsageBit["INDIRECT"] = 64] = "INDIRECT";
            })(BufferUsageBit || (BufferUsageBit = {}));
            exports_16("BufferUsageBit", BufferUsageBit);
            (function (BufferFlagBit) {
                BufferFlagBit[BufferFlagBit["NONE"] = 0] = "NONE";
            })(BufferFlagBit || (BufferFlagBit = {}));
            exports_16("BufferFlagBit", BufferFlagBit);
            (function (MemoryAccessBit) {
                MemoryAccessBit[MemoryAccessBit["NONE"] = 0] = "NONE";
                MemoryAccessBit[MemoryAccessBit["READ_ONLY"] = 1] = "READ_ONLY";
                MemoryAccessBit[MemoryAccessBit["WRITE_ONLY"] = 2] = "WRITE_ONLY";
                MemoryAccessBit[MemoryAccessBit["READ_WRITE"] = 3] = "READ_WRITE";
            })(MemoryAccessBit || (MemoryAccessBit = {}));
            exports_16("MemoryAccessBit", MemoryAccessBit);
            (function (MemoryUsageBit) {
                MemoryUsageBit[MemoryUsageBit["NONE"] = 0] = "NONE";
                MemoryUsageBit[MemoryUsageBit["DEVICE"] = 1] = "DEVICE";
                MemoryUsageBit[MemoryUsageBit["HOST"] = 2] = "HOST";
            })(MemoryUsageBit || (MemoryUsageBit = {}));
            exports_16("MemoryUsageBit", MemoryUsageBit);
            (function (TextureType) {
                TextureType[TextureType["TEX1D"] = 0] = "TEX1D";
                TextureType[TextureType["TEX2D"] = 1] = "TEX2D";
                TextureType[TextureType["TEX3D"] = 2] = "TEX3D";
                TextureType[TextureType["CUBE"] = 3] = "CUBE";
                TextureType[TextureType["TEX1D_ARRAY"] = 4] = "TEX1D_ARRAY";
                TextureType[TextureType["TEX2D_ARRAY"] = 5] = "TEX2D_ARRAY";
            })(TextureType || (TextureType = {}));
            exports_16("TextureType", TextureType);
            (function (TextureUsageBit) {
                TextureUsageBit[TextureUsageBit["NONE"] = 0] = "NONE";
                TextureUsageBit[TextureUsageBit["TRANSFER_SRC"] = 1] = "TRANSFER_SRC";
                TextureUsageBit[TextureUsageBit["TRANSFER_DST"] = 2] = "TRANSFER_DST";
                TextureUsageBit[TextureUsageBit["SAMPLED"] = 4] = "SAMPLED";
                TextureUsageBit[TextureUsageBit["STORAGE"] = 8] = "STORAGE";
                TextureUsageBit[TextureUsageBit["COLOR_ATTACHMENT"] = 16] = "COLOR_ATTACHMENT";
                TextureUsageBit[TextureUsageBit["DEPTH_STENCIL_ATTACHMENT"] = 32] = "DEPTH_STENCIL_ATTACHMENT";
                TextureUsageBit[TextureUsageBit["INPUT_ATTACHMENT"] = 64] = "INPUT_ATTACHMENT";
            })(TextureUsageBit || (TextureUsageBit = {}));
            exports_16("TextureUsageBit", TextureUsageBit);
            (function (TextureFlagBit) {
                TextureFlagBit[TextureFlagBit["NONE"] = 0] = "NONE";
                TextureFlagBit[TextureFlagBit["GEN_MIPMAP"] = 1] = "GEN_MIPMAP";
                TextureFlagBit[TextureFlagBit["GENERAL_LAYOUT"] = 2] = "GENERAL_LAYOUT";
            })(TextureFlagBit || (TextureFlagBit = {}));
            exports_16("TextureFlagBit", TextureFlagBit);
            (function (SampleCount) {
                SampleCount[SampleCount["ONE"] = 0] = "ONE";
                SampleCount[SampleCount["MULTIPLE_PERFORMANCE"] = 1] = "MULTIPLE_PERFORMANCE";
                SampleCount[SampleCount["MULTIPLE_BALANCE"] = 2] = "MULTIPLE_BALANCE";
                SampleCount[SampleCount["MULTIPLE_QUALITY"] = 3] = "MULTIPLE_QUALITY";
            })(SampleCount || (SampleCount = {}));
            exports_16("SampleCount", SampleCount);
            (function (VsyncMode) {
                // The application does not synchronizes with the vertical sync.
                // If application renders faster than the display refreshes, frames are wasted and tearing may be observed.
                // FPS is uncapped. Maximum power consumption. If unsupported, "ON" value will be used instead. Minimum latency.
                VsyncMode[VsyncMode["OFF"] = 0] = "OFF";
                // The application is always synchronized with the vertical sync. Tearing does not happen.
                // FPS is capped to the display's refresh rate. For fast applications, battery life is improved. Always supported.
                VsyncMode[VsyncMode["ON"] = 1] = "ON";
                // The application synchronizes with the vertical sync, but only if the application rendering speed is greater than refresh rate.
                // Compared to OFF, there is no tearing. Compared to ON, the FPS will be improved for "slower" applications.
                // If unsupported, "ON" value will be used instead. Recommended for most applications. Default if supported.
                VsyncMode[VsyncMode["RELAXED"] = 2] = "RELAXED";
                // The presentation engine will always use the latest fully rendered image.
                // Compared to OFF, no tearing will be observed.
                // Compared to ON, battery power will be worse, especially for faster applications.
                // If unsupported,  "OFF" will be attempted next.
                VsyncMode[VsyncMode["MAILBOX"] = 3] = "MAILBOX";
                // The application is capped to using half the vertical sync time.
                // FPS artificially capped to Half the display speed (usually 30fps) to maintain battery.
                // Best possible battery savings. Worst possible performance.
                // Recommended for specific applications where battery saving is critical.
                VsyncMode[VsyncMode["HALF"] = 4] = "HALF";
            })(VsyncMode || (VsyncMode = {}));
            exports_16("VsyncMode", VsyncMode);
            (function (Filter) {
                Filter[Filter["NONE"] = 0] = "NONE";
                Filter[Filter["POINT"] = 1] = "POINT";
                Filter[Filter["LINEAR"] = 2] = "LINEAR";
                Filter[Filter["ANISOTROPIC"] = 3] = "ANISOTROPIC";
            })(Filter || (Filter = {}));
            exports_16("Filter", Filter);
            (function (Address) {
                Address[Address["WRAP"] = 0] = "WRAP";
                Address[Address["MIRROR"] = 1] = "MIRROR";
                Address[Address["CLAMP"] = 2] = "CLAMP";
                Address[Address["BORDER"] = 3] = "BORDER";
            })(Address || (Address = {}));
            exports_16("Address", Address);
            (function (ComparisonFunc) {
                ComparisonFunc[ComparisonFunc["NEVER"] = 0] = "NEVER";
                ComparisonFunc[ComparisonFunc["LESS"] = 1] = "LESS";
                ComparisonFunc[ComparisonFunc["EQUAL"] = 2] = "EQUAL";
                ComparisonFunc[ComparisonFunc["LESS_EQUAL"] = 3] = "LESS_EQUAL";
                ComparisonFunc[ComparisonFunc["GREATER"] = 4] = "GREATER";
                ComparisonFunc[ComparisonFunc["NOT_EQUAL"] = 5] = "NOT_EQUAL";
                ComparisonFunc[ComparisonFunc["GREATER_EQUAL"] = 6] = "GREATER_EQUAL";
                ComparisonFunc[ComparisonFunc["ALWAYS"] = 7] = "ALWAYS";
            })(ComparisonFunc || (ComparisonFunc = {}));
            exports_16("ComparisonFunc", ComparisonFunc);
            (function (StencilOp) {
                StencilOp[StencilOp["ZERO"] = 0] = "ZERO";
                StencilOp[StencilOp["KEEP"] = 1] = "KEEP";
                StencilOp[StencilOp["REPLACE"] = 2] = "REPLACE";
                StencilOp[StencilOp["INCR"] = 3] = "INCR";
                StencilOp[StencilOp["DECR"] = 4] = "DECR";
                StencilOp[StencilOp["INVERT"] = 5] = "INVERT";
                StencilOp[StencilOp["INCR_WRAP"] = 6] = "INCR_WRAP";
                StencilOp[StencilOp["DECR_WRAP"] = 7] = "DECR_WRAP";
            })(StencilOp || (StencilOp = {}));
            exports_16("StencilOp", StencilOp);
            (function (BlendFactor) {
                BlendFactor[BlendFactor["ZERO"] = 0] = "ZERO";
                BlendFactor[BlendFactor["ONE"] = 1] = "ONE";
                BlendFactor[BlendFactor["SRC_ALPHA"] = 2] = "SRC_ALPHA";
                BlendFactor[BlendFactor["DST_ALPHA"] = 3] = "DST_ALPHA";
                BlendFactor[BlendFactor["ONE_MINUS_SRC_ALPHA"] = 4] = "ONE_MINUS_SRC_ALPHA";
                BlendFactor[BlendFactor["ONE_MINUS_DST_ALPHA"] = 5] = "ONE_MINUS_DST_ALPHA";
                BlendFactor[BlendFactor["SRC_COLOR"] = 6] = "SRC_COLOR";
                BlendFactor[BlendFactor["DST_COLOR"] = 7] = "DST_COLOR";
                BlendFactor[BlendFactor["ONE_MINUS_SRC_COLOR"] = 8] = "ONE_MINUS_SRC_COLOR";
                BlendFactor[BlendFactor["ONE_MINUS_DST_COLOR"] = 9] = "ONE_MINUS_DST_COLOR";
                BlendFactor[BlendFactor["SRC_ALPHA_SATURATE"] = 10] = "SRC_ALPHA_SATURATE";
                BlendFactor[BlendFactor["CONSTANT_COLOR"] = 11] = "CONSTANT_COLOR";
                BlendFactor[BlendFactor["ONE_MINUS_CONSTANT_COLOR"] = 12] = "ONE_MINUS_CONSTANT_COLOR";
                BlendFactor[BlendFactor["CONSTANT_ALPHA"] = 13] = "CONSTANT_ALPHA";
                BlendFactor[BlendFactor["ONE_MINUS_CONSTANT_ALPHA"] = 14] = "ONE_MINUS_CONSTANT_ALPHA";
            })(BlendFactor || (BlendFactor = {}));
            exports_16("BlendFactor", BlendFactor);
            (function (BlendOp) {
                BlendOp[BlendOp["ADD"] = 0] = "ADD";
                BlendOp[BlendOp["SUB"] = 1] = "SUB";
                BlendOp[BlendOp["REV_SUB"] = 2] = "REV_SUB";
                BlendOp[BlendOp["MIN"] = 3] = "MIN";
                BlendOp[BlendOp["MAX"] = 4] = "MAX";
            })(BlendOp || (BlendOp = {}));
            exports_16("BlendOp", BlendOp);
            (function (ColorMask) {
                ColorMask[ColorMask["NONE"] = 0] = "NONE";
                ColorMask[ColorMask["R"] = 1] = "R";
                ColorMask[ColorMask["G"] = 2] = "G";
                ColorMask[ColorMask["B"] = 4] = "B";
                ColorMask[ColorMask["A"] = 8] = "A";
                ColorMask[ColorMask["ALL"] = 15] = "ALL";
            })(ColorMask || (ColorMask = {}));
            exports_16("ColorMask", ColorMask);
            (function (ShaderStageFlagBit) {
                ShaderStageFlagBit[ShaderStageFlagBit["NONE"] = 0] = "NONE";
                ShaderStageFlagBit[ShaderStageFlagBit["VERTEX"] = 1] = "VERTEX";
                ShaderStageFlagBit[ShaderStageFlagBit["CONTROL"] = 2] = "CONTROL";
                ShaderStageFlagBit[ShaderStageFlagBit["EVALUATION"] = 4] = "EVALUATION";
                ShaderStageFlagBit[ShaderStageFlagBit["GEOMETRY"] = 8] = "GEOMETRY";
                ShaderStageFlagBit[ShaderStageFlagBit["FRAGMENT"] = 16] = "FRAGMENT";
                ShaderStageFlagBit[ShaderStageFlagBit["COMPUTE"] = 32] = "COMPUTE";
                ShaderStageFlagBit[ShaderStageFlagBit["ALL"] = 63] = "ALL";
            })(ShaderStageFlagBit || (ShaderStageFlagBit = {}));
            exports_16("ShaderStageFlagBit", ShaderStageFlagBit);
            (function (LoadOp) {
                LoadOp[LoadOp["LOAD"] = 0] = "LOAD";
                LoadOp[LoadOp["CLEAR"] = 1] = "CLEAR";
                LoadOp[LoadOp["DISCARD"] = 2] = "DISCARD";
            })(LoadOp || (LoadOp = {}));
            exports_16("LoadOp", LoadOp);
            (function (StoreOp) {
                StoreOp[StoreOp["STORE"] = 0] = "STORE";
                StoreOp[StoreOp["DISCARD"] = 1] = "DISCARD";
            })(StoreOp || (StoreOp = {}));
            exports_16("StoreOp", StoreOp);
            (function (AccessType) {
                AccessType[AccessType["NONE"] = 0] = "NONE";
                // Read access
                AccessType[AccessType["INDIRECT_BUFFER"] = 1] = "INDIRECT_BUFFER";
                AccessType[AccessType["INDEX_BUFFER"] = 2] = "INDEX_BUFFER";
                AccessType[AccessType["VERTEX_BUFFER"] = 3] = "VERTEX_BUFFER";
                AccessType[AccessType["VERTEX_SHADER_READ_UNIFORM_BUFFER"] = 4] = "VERTEX_SHADER_READ_UNIFORM_BUFFER";
                AccessType[AccessType["VERTEX_SHADER_READ_TEXTURE"] = 5] = "VERTEX_SHADER_READ_TEXTURE";
                AccessType[AccessType["VERTEX_SHADER_READ_OTHER"] = 6] = "VERTEX_SHADER_READ_OTHER";
                AccessType[AccessType["FRAGMENT_SHADER_READ_UNIFORM_BUFFER"] = 7] = "FRAGMENT_SHADER_READ_UNIFORM_BUFFER";
                AccessType[AccessType["FRAGMENT_SHADER_READ_TEXTURE"] = 8] = "FRAGMENT_SHADER_READ_TEXTURE";
                AccessType[AccessType["FRAGMENT_SHADER_READ_COLOR_INPUT_ATTACHMENT"] = 9] = "FRAGMENT_SHADER_READ_COLOR_INPUT_ATTACHMENT";
                AccessType[AccessType["FRAGMENT_SHADER_READ_DEPTH_STENCIL_INPUT_ATTACHMENT"] = 10] = "FRAGMENT_SHADER_READ_DEPTH_STENCIL_INPUT_ATTACHMENT";
                AccessType[AccessType["FRAGMENT_SHADER_READ_OTHER"] = 11] = "FRAGMENT_SHADER_READ_OTHER";
                AccessType[AccessType["COLOR_ATTACHMENT_READ"] = 12] = "COLOR_ATTACHMENT_READ";
                AccessType[AccessType["DEPTH_STENCIL_ATTACHMENT_READ"] = 13] = "DEPTH_STENCIL_ATTACHMENT_READ";
                AccessType[AccessType["COMPUTE_SHADER_READ_UNIFORM_BUFFER"] = 14] = "COMPUTE_SHADER_READ_UNIFORM_BUFFER";
                AccessType[AccessType["COMPUTE_SHADER_READ_TEXTURE"] = 15] = "COMPUTE_SHADER_READ_TEXTURE";
                AccessType[AccessType["COMPUTE_SHADER_READ_OTHER"] = 16] = "COMPUTE_SHADER_READ_OTHER";
                AccessType[AccessType["TRANSFER_READ"] = 17] = "TRANSFER_READ";
                AccessType[AccessType["HOST_READ"] = 18] = "HOST_READ";
                AccessType[AccessType["PRESENT"] = 19] = "PRESENT";
                // Write access
                AccessType[AccessType["VERTEX_SHADER_WRITE"] = 20] = "VERTEX_SHADER_WRITE";
                AccessType[AccessType["FRAGMENT_SHADER_WRITE"] = 21] = "FRAGMENT_SHADER_WRITE";
                AccessType[AccessType["COLOR_ATTACHMENT_WRITE"] = 22] = "COLOR_ATTACHMENT_WRITE";
                AccessType[AccessType["DEPTH_STENCIL_ATTACHMENT_WRITE"] = 23] = "DEPTH_STENCIL_ATTACHMENT_WRITE";
                AccessType[AccessType["COMPUTE_SHADER_WRITE"] = 24] = "COMPUTE_SHADER_WRITE";
                AccessType[AccessType["TRANSFER_WRITE"] = 25] = "TRANSFER_WRITE";
                AccessType[AccessType["HOST_PREINITIALIZED"] = 26] = "HOST_PREINITIALIZED";
                AccessType[AccessType["HOST_WRITE"] = 27] = "HOST_WRITE";
            })(AccessType || (AccessType = {}));
            exports_16("AccessType", AccessType);
            (function (ResolveMode) {
                ResolveMode[ResolveMode["NONE"] = 0] = "NONE";
                ResolveMode[ResolveMode["SAMPLE_ZERO"] = 1] = "SAMPLE_ZERO";
                ResolveMode[ResolveMode["AVERAGE"] = 2] = "AVERAGE";
                ResolveMode[ResolveMode["MIN"] = 3] = "MIN";
                ResolveMode[ResolveMode["MAX"] = 4] = "MAX";
            })(ResolveMode || (ResolveMode = {}));
            exports_16("ResolveMode", ResolveMode);
            (function (PipelineBindPoint) {
                PipelineBindPoint[PipelineBindPoint["GRAPHICS"] = 0] = "GRAPHICS";
                PipelineBindPoint[PipelineBindPoint["COMPUTE"] = 1] = "COMPUTE";
                PipelineBindPoint[PipelineBindPoint["RAY_TRACING"] = 2] = "RAY_TRACING";
            })(PipelineBindPoint || (PipelineBindPoint = {}));
            exports_16("PipelineBindPoint", PipelineBindPoint);
            (function (PrimitiveMode) {
                PrimitiveMode[PrimitiveMode["POINT_LIST"] = 0] = "POINT_LIST";
                PrimitiveMode[PrimitiveMode["LINE_LIST"] = 1] = "LINE_LIST";
                PrimitiveMode[PrimitiveMode["LINE_STRIP"] = 2] = "LINE_STRIP";
                PrimitiveMode[PrimitiveMode["LINE_LOOP"] = 3] = "LINE_LOOP";
                PrimitiveMode[PrimitiveMode["LINE_LIST_ADJACENCY"] = 4] = "LINE_LIST_ADJACENCY";
                PrimitiveMode[PrimitiveMode["LINE_STRIP_ADJACENCY"] = 5] = "LINE_STRIP_ADJACENCY";
                PrimitiveMode[PrimitiveMode["ISO_LINE_LIST"] = 6] = "ISO_LINE_LIST";
                // raycast detectable:
                PrimitiveMode[PrimitiveMode["TRIANGLE_LIST"] = 7] = "TRIANGLE_LIST";
                PrimitiveMode[PrimitiveMode["TRIANGLE_STRIP"] = 8] = "TRIANGLE_STRIP";
                PrimitiveMode[PrimitiveMode["TRIANGLE_FAN"] = 9] = "TRIANGLE_FAN";
                PrimitiveMode[PrimitiveMode["TRIANGLE_LIST_ADJACENCY"] = 10] = "TRIANGLE_LIST_ADJACENCY";
                PrimitiveMode[PrimitiveMode["TRIANGLE_STRIP_ADJACENCY"] = 11] = "TRIANGLE_STRIP_ADJACENCY";
                PrimitiveMode[PrimitiveMode["TRIANGLE_PATCH_ADJACENCY"] = 12] = "TRIANGLE_PATCH_ADJACENCY";
                PrimitiveMode[PrimitiveMode["QUAD_PATCH_LIST"] = 13] = "QUAD_PATCH_LIST";
            })(PrimitiveMode || (PrimitiveMode = {}));
            exports_16("PrimitiveMode", PrimitiveMode);
            (function (PolygonMode) {
                PolygonMode[PolygonMode["FILL"] = 0] = "FILL";
                PolygonMode[PolygonMode["POINT"] = 1] = "POINT";
                PolygonMode[PolygonMode["LINE"] = 2] = "LINE";
            })(PolygonMode || (PolygonMode = {}));
            exports_16("PolygonMode", PolygonMode);
            (function (ShadeModel) {
                ShadeModel[ShadeModel["GOURAND"] = 0] = "GOURAND";
                ShadeModel[ShadeModel["FLAT"] = 1] = "FLAT";
            })(ShadeModel || (ShadeModel = {}));
            exports_16("ShadeModel", ShadeModel);
            (function (CullMode) {
                CullMode[CullMode["NONE"] = 0] = "NONE";
                CullMode[CullMode["FRONT"] = 1] = "FRONT";
                CullMode[CullMode["BACK"] = 2] = "BACK";
            })(CullMode || (CullMode = {}));
            exports_16("CullMode", CullMode);
            (function (DynamicStateFlagBit) {
                DynamicStateFlagBit[DynamicStateFlagBit["NONE"] = 0] = "NONE";
                DynamicStateFlagBit[DynamicStateFlagBit["LINE_WIDTH"] = 1] = "LINE_WIDTH";
                DynamicStateFlagBit[DynamicStateFlagBit["DEPTH_BIAS"] = 2] = "DEPTH_BIAS";
                DynamicStateFlagBit[DynamicStateFlagBit["BLEND_CONSTANTS"] = 4] = "BLEND_CONSTANTS";
                DynamicStateFlagBit[DynamicStateFlagBit["DEPTH_BOUNDS"] = 8] = "DEPTH_BOUNDS";
                DynamicStateFlagBit[DynamicStateFlagBit["STENCIL_WRITE_MASK"] = 16] = "STENCIL_WRITE_MASK";
                DynamicStateFlagBit[DynamicStateFlagBit["STENCIL_COMPARE_MASK"] = 32] = "STENCIL_COMPARE_MASK";
            })(DynamicStateFlagBit || (DynamicStateFlagBit = {}));
            exports_16("DynamicStateFlagBit", DynamicStateFlagBit);
            (function (StencilFace) {
                StencilFace[StencilFace["FRONT"] = 1] = "FRONT";
                StencilFace[StencilFace["BACK"] = 2] = "BACK";
                StencilFace[StencilFace["ALL"] = 3] = "ALL";
            })(StencilFace || (StencilFace = {}));
            exports_16("StencilFace", StencilFace);
            (function (DescriptorType) {
                DescriptorType[DescriptorType["UNKNOWN"] = 0] = "UNKNOWN";
                DescriptorType[DescriptorType["UNIFORM_BUFFER"] = 1] = "UNIFORM_BUFFER";
                DescriptorType[DescriptorType["DYNAMIC_UNIFORM_BUFFER"] = 2] = "DYNAMIC_UNIFORM_BUFFER";
                DescriptorType[DescriptorType["STORAGE_BUFFER"] = 4] = "STORAGE_BUFFER";
                DescriptorType[DescriptorType["DYNAMIC_STORAGE_BUFFER"] = 8] = "DYNAMIC_STORAGE_BUFFER";
                DescriptorType[DescriptorType["SAMPLER_TEXTURE"] = 16] = "SAMPLER_TEXTURE";
                DescriptorType[DescriptorType["SAMPLER"] = 32] = "SAMPLER";
                DescriptorType[DescriptorType["TEXTURE"] = 64] = "TEXTURE";
                DescriptorType[DescriptorType["STORAGE_IMAGE"] = 128] = "STORAGE_IMAGE";
                DescriptorType[DescriptorType["INPUT_ATTACHMENT"] = 256] = "INPUT_ATTACHMENT";
            })(DescriptorType || (DescriptorType = {}));
            exports_16("DescriptorType", DescriptorType);
            (function (QueueType) {
                QueueType[QueueType["GRAPHICS"] = 0] = "GRAPHICS";
                QueueType[QueueType["COMPUTE"] = 1] = "COMPUTE";
                QueueType[QueueType["TRANSFER"] = 2] = "TRANSFER";
            })(QueueType || (QueueType = {}));
            exports_16("QueueType", QueueType);
            (function (CommandBufferType) {
                CommandBufferType[CommandBufferType["PRIMARY"] = 0] = "PRIMARY";
                CommandBufferType[CommandBufferType["SECONDARY"] = 1] = "SECONDARY";
            })(CommandBufferType || (CommandBufferType = {}));
            exports_16("CommandBufferType", CommandBufferType);
            (function (ClearFlagBit) {
                ClearFlagBit[ClearFlagBit["NONE"] = 0] = "NONE";
                ClearFlagBit[ClearFlagBit["COLOR"] = 1] = "COLOR";
                ClearFlagBit[ClearFlagBit["DEPTH"] = 2] = "DEPTH";
                ClearFlagBit[ClearFlagBit["STENCIL"] = 4] = "STENCIL";
                ClearFlagBit[ClearFlagBit["DEPTH_STENCIL"] = 6] = "DEPTH_STENCIL";
                ClearFlagBit[ClearFlagBit["ALL"] = 7] = "ALL";
            })(ClearFlagBit || (ClearFlagBit = {}));
            exports_16("ClearFlagBit", ClearFlagBit);
            Size = class Size {
                constructor(x = 0, y = 0, z = 0) {
                    this.x = x;
                    this.y = y;
                    this.z = z;
                }
                copy(info) {
                    this.x = info.x;
                    this.y = info.y;
                    this.z = info.z;
                    return this;
                }
            };
            exports_16("Size", Size);
            DeviceCaps = class DeviceCaps {
                constructor(maxVertexAttributes = 0, maxVertexUniformVectors = 0, maxFragmentUniformVectors = 0, maxTextureUnits = 0, maxImageUnits = 0, maxVertexTextureUnits = 0, maxColorRenderTargets = 0, maxShaderStorageBufferBindings = 0, maxShaderStorageBlockSize = 0, maxUniformBufferBindings = 0, maxUniformBlockSize = 0, maxTextureSize = 0, maxCubeMapTextureSize = 0, uboOffsetAlignment = 1, maxComputeSharedMemorySize = 0, maxComputeWorkGroupInvocations = 0, maxComputeWorkGroupSize = new Size(), maxComputeWorkGroupCount = new Size(), supportQuery = 0, clipSpaceMinZ = -1, screenSpaceSignY = 1, clipSpaceSignY = 1) {
                    this.maxVertexAttributes = maxVertexAttributes;
                    this.maxVertexUniformVectors = maxVertexUniformVectors;
                    this.maxFragmentUniformVectors = maxFragmentUniformVectors;
                    this.maxTextureUnits = maxTextureUnits;
                    this.maxImageUnits = maxImageUnits;
                    this.maxVertexTextureUnits = maxVertexTextureUnits;
                    this.maxColorRenderTargets = maxColorRenderTargets;
                    this.maxShaderStorageBufferBindings = maxShaderStorageBufferBindings;
                    this.maxShaderStorageBlockSize = maxShaderStorageBlockSize;
                    this.maxUniformBufferBindings = maxUniformBufferBindings;
                    this.maxUniformBlockSize = maxUniformBlockSize;
                    this.maxTextureSize = maxTextureSize;
                    this.maxCubeMapTextureSize = maxCubeMapTextureSize;
                    this.uboOffsetAlignment = uboOffsetAlignment;
                    this.maxComputeSharedMemorySize = maxComputeSharedMemorySize;
                    this.maxComputeWorkGroupInvocations = maxComputeWorkGroupInvocations;
                    this.maxComputeWorkGroupSize = maxComputeWorkGroupSize;
                    this.maxComputeWorkGroupCount = maxComputeWorkGroupCount;
                    this.supportQuery = supportQuery;
                    this.clipSpaceMinZ = clipSpaceMinZ;
                    this.screenSpaceSignY = screenSpaceSignY;
                    this.clipSpaceSignY = clipSpaceSignY;
                }
                copy(info) {
                    this.maxVertexAttributes = info.maxVertexAttributes;
                    this.maxVertexUniformVectors = info.maxVertexUniformVectors;
                    this.maxFragmentUniformVectors = info.maxFragmentUniformVectors;
                    this.maxTextureUnits = info.maxTextureUnits;
                    this.maxImageUnits = info.maxImageUnits;
                    this.maxVertexTextureUnits = info.maxVertexTextureUnits;
                    this.maxColorRenderTargets = info.maxColorRenderTargets;
                    this.maxShaderStorageBufferBindings = info.maxShaderStorageBufferBindings;
                    this.maxShaderStorageBlockSize = info.maxShaderStorageBlockSize;
                    this.maxUniformBufferBindings = info.maxUniformBufferBindings;
                    this.maxUniformBlockSize = info.maxUniformBlockSize;
                    this.maxTextureSize = info.maxTextureSize;
                    this.maxCubeMapTextureSize = info.maxCubeMapTextureSize;
                    this.uboOffsetAlignment = info.uboOffsetAlignment;
                    this.maxComputeSharedMemorySize = info.maxComputeSharedMemorySize;
                    this.maxComputeWorkGroupInvocations = info.maxComputeWorkGroupInvocations;
                    this.maxComputeWorkGroupSize.copy(info.maxComputeWorkGroupSize);
                    this.maxComputeWorkGroupCount.copy(info.maxComputeWorkGroupCount);
                    this.supportQuery = info.supportQuery;
                    this.clipSpaceMinZ = info.clipSpaceMinZ;
                    this.screenSpaceSignY = info.screenSpaceSignY;
                    this.clipSpaceSignY = info.clipSpaceSignY;
                    return this;
                }
            };
            exports_16("DeviceCaps", DeviceCaps);
            Offset = class Offset {
                constructor(x = 0, y = 0, z = 0) {
                    this.x = x;
                    this.y = y;
                    this.z = z;
                }
                copy(info) {
                    this.x = info.x;
                    this.y = info.y;
                    this.z = info.z;
                    return this;
                }
            };
            exports_16("Offset", Offset);
            Rect = class Rect {
                constructor(x = 0, y = 0, width = 0, height = 0) {
                    this.x = x;
                    this.y = y;
                    this.width = width;
                    this.height = height;
                }
                copy(info) {
                    this.x = info.x;
                    this.y = info.y;
                    this.width = info.width;
                    this.height = info.height;
                    return this;
                }
            };
            exports_16("Rect", Rect);
            Extent = class Extent {
                constructor(width = 0, height = 0, depth = 1) {
                    this.width = width;
                    this.height = height;
                    this.depth = depth;
                }
                copy(info) {
                    this.width = info.width;
                    this.height = info.height;
                    this.depth = info.depth;
                    return this;
                }
            };
            exports_16("Extent", Extent);
            TextureSubresLayers = class TextureSubresLayers {
                constructor(mipLevel = 0, baseArrayLayer = 0, layerCount = 1) {
                    this.mipLevel = mipLevel;
                    this.baseArrayLayer = baseArrayLayer;
                    this.layerCount = layerCount;
                }
                copy(info) {
                    this.mipLevel = info.mipLevel;
                    this.baseArrayLayer = info.baseArrayLayer;
                    this.layerCount = info.layerCount;
                    return this;
                }
            };
            exports_16("TextureSubresLayers", TextureSubresLayers);
            TextureSubresRange = class TextureSubresRange {
                constructor(baseMipLevel = 0, levelCount = 1, baseArrayLayer = 0, layerCount = 1) {
                    this.baseMipLevel = baseMipLevel;
                    this.levelCount = levelCount;
                    this.baseArrayLayer = baseArrayLayer;
                    this.layerCount = layerCount;
                }
                copy(info) {
                    this.baseMipLevel = info.baseMipLevel;
                    this.levelCount = info.levelCount;
                    this.baseArrayLayer = info.baseArrayLayer;
                    this.layerCount = info.layerCount;
                    return this;
                }
            };
            exports_16("TextureSubresRange", TextureSubresRange);
            TextureCopy = class TextureCopy {
                constructor(srcSubres = new TextureSubresLayers(), srcOffset = new Offset(), dstSubres = new TextureSubresLayers(), dstOffset = new Offset(), extent = new Extent()) {
                    this.srcSubres = srcSubres;
                    this.srcOffset = srcOffset;
                    this.dstSubres = dstSubres;
                    this.dstOffset = dstOffset;
                    this.extent = extent;
                }
                copy(info) {
                    this.srcSubres.copy(info.srcSubres);
                    this.srcOffset.copy(info.srcOffset);
                    this.dstSubres.copy(info.dstSubres);
                    this.dstOffset.copy(info.dstOffset);
                    this.extent.copy(info.extent);
                    return this;
                }
            };
            exports_16("TextureCopy", TextureCopy);
            TextureBlit = class TextureBlit {
                constructor(srcSubres = new TextureSubresLayers(), srcOffset = new Offset(), srcExtent = new Extent(), dstSubres = new TextureSubresLayers(), dstOffset = new Offset(), dstExtent = new Extent()) {
                    this.srcSubres = srcSubres;
                    this.srcOffset = srcOffset;
                    this.srcExtent = srcExtent;
                    this.dstSubres = dstSubres;
                    this.dstOffset = dstOffset;
                    this.dstExtent = dstExtent;
                }
                copy(info) {
                    this.srcSubres.copy(info.srcSubres);
                    this.srcOffset.copy(info.srcOffset);
                    this.srcExtent.copy(info.srcExtent);
                    this.dstSubres.copy(info.dstSubres);
                    this.dstOffset.copy(info.dstOffset);
                    this.dstExtent.copy(info.dstExtent);
                    return this;
                }
            };
            exports_16("TextureBlit", TextureBlit);
            BufferTextureCopy = class BufferTextureCopy {
                constructor(buffStride = 0, buffTexHeight = 0, texOffset = new Offset(), texExtent = new Extent(), texSubres = new TextureSubresLayers()) {
                    this.buffStride = buffStride;
                    this.buffTexHeight = buffTexHeight;
                    this.texOffset = texOffset;
                    this.texExtent = texExtent;
                    this.texSubres = texSubres;
                }
                copy(info) {
                    this.buffStride = info.buffStride;
                    this.buffTexHeight = info.buffTexHeight;
                    this.texOffset.copy(info.texOffset);
                    this.texExtent.copy(info.texExtent);
                    this.texSubres.copy(info.texSubres);
                    return this;
                }
            };
            exports_16("BufferTextureCopy", BufferTextureCopy);
            Viewport = class Viewport {
                constructor(left = 0, top = 0, width = 0, height = 0, minDepth = 0, maxDepth = 1) {
                    this.left = left;
                    this.top = top;
                    this.width = width;
                    this.height = height;
                    this.minDepth = minDepth;
                    this.maxDepth = maxDepth;
                }
                copy(info) {
                    this.left = info.left;
                    this.top = info.top;
                    this.width = info.width;
                    this.height = info.height;
                    this.minDepth = info.minDepth;
                    this.maxDepth = info.maxDepth;
                    return this;
                }
            };
            exports_16("Viewport", Viewport);
            Color = class Color {
                constructor(x = 0, y = 0, z = 0, w = 0) {
                    this.x = x;
                    this.y = y;
                    this.z = z;
                    this.w = w;
                }
                copy(info) {
                    this.x = info.x;
                    this.y = info.y;
                    this.z = info.z;
                    this.w = info.w;
                    return this;
                }
            };
            exports_16("Color", Color);
            BindingMappingInfo = class BindingMappingInfo {
                constructor(bufferOffsets = [], samplerOffsets = [], flexibleSet = 0) {
                    this.bufferOffsets = bufferOffsets;
                    this.samplerOffsets = samplerOffsets;
                    this.flexibleSet = flexibleSet;
                }
                copy(info) {
                    this.bufferOffsets = info.bufferOffsets.slice();
                    this.samplerOffsets = info.samplerOffsets.slice();
                    this.flexibleSet = info.flexibleSet;
                    return this;
                }
            };
            exports_16("BindingMappingInfo", BindingMappingInfo);
            SwapchainInfo = class SwapchainInfo {
                constructor(windowHandle = null, vsyncMode = VsyncMode.ON, width = 0, height = 0) {
                    this.windowHandle = windowHandle;
                    this.vsyncMode = vsyncMode;
                    this.width = width;
                    this.height = height;
                }
                copy(info) {
                    this.windowHandle = info.windowHandle;
                    this.vsyncMode = info.vsyncMode;
                    this.width = info.width;
                    this.height = info.height;
                    return this;
                }
            };
            exports_16("SwapchainInfo", SwapchainInfo);
            DeviceInfo = class DeviceInfo {
                constructor(bindingMappingInfo = new BindingMappingInfo()) {
                    this.bindingMappingInfo = bindingMappingInfo;
                }
                copy(info) {
                    this.bindingMappingInfo.copy(info.bindingMappingInfo);
                    return this;
                }
            };
            exports_16("DeviceInfo", DeviceInfo);
            BufferInfo = class BufferInfo {
                constructor(usage = BufferUsageBit.NONE, memUsage = MemoryUsageBit.NONE, size = 0, stride = 0, flags = BufferFlagBit.NONE) {
                    this.usage = usage;
                    this.memUsage = memUsage;
                    this.size = size;
                    this.stride = stride;
                    this.flags = flags;
                }
                copy(info) {
                    this.usage = info.usage;
                    this.memUsage = info.memUsage;
                    this.size = info.size;
                    this.stride = info.stride;
                    this.flags = info.flags;
                    return this;
                }
            };
            exports_16("BufferInfo", BufferInfo);
            BufferViewInfo = class BufferViewInfo {
                constructor(buffer = null, offset = 0, range = 0) {
                    this.buffer = buffer;
                    this.offset = offset;
                    this.range = range;
                }
                copy(info) {
                    this.buffer = info.buffer;
                    this.offset = info.offset;
                    this.range = info.range;
                    return this;
                }
            };
            exports_16("BufferViewInfo", BufferViewInfo);
            DrawInfo = class DrawInfo {
                constructor(vertexCount = 0, firstVertex = 0, indexCount = 0, firstIndex = 0, vertexOffset = 0, instanceCount = 0, firstInstance = 0) {
                    this.vertexCount = vertexCount;
                    this.firstVertex = firstVertex;
                    this.indexCount = indexCount;
                    this.firstIndex = firstIndex;
                    this.vertexOffset = vertexOffset;
                    this.instanceCount = instanceCount;
                    this.firstInstance = firstInstance;
                }
                copy(info) {
                    this.vertexCount = info.vertexCount;
                    this.firstVertex = info.firstVertex;
                    this.indexCount = info.indexCount;
                    this.firstIndex = info.firstIndex;
                    this.vertexOffset = info.vertexOffset;
                    this.instanceCount = info.instanceCount;
                    this.firstInstance = info.firstInstance;
                    return this;
                }
            };
            exports_16("DrawInfo", DrawInfo);
            DispatchInfo = class DispatchInfo {
                constructor(groupCountX = 0, groupCountY = 0, groupCountZ = 0, indirectBuffer = null, indirectOffset = 0) {
                    this.groupCountX = groupCountX;
                    this.groupCountY = groupCountY;
                    this.groupCountZ = groupCountZ;
                    this.indirectBuffer = indirectBuffer;
                    this.indirectOffset = indirectOffset;
                }
                copy(info) {
                    this.groupCountX = info.groupCountX;
                    this.groupCountY = info.groupCountY;
                    this.groupCountZ = info.groupCountZ;
                    this.indirectBuffer = info.indirectBuffer;
                    this.indirectOffset = info.indirectOffset;
                    return this;
                }
            };
            exports_16("DispatchInfo", DispatchInfo);
            IndirectBuffer = class IndirectBuffer {
                constructor(drawInfos = []) {
                    this.drawInfos = drawInfos;
                }
                copy(info) {
                    deepCopy(this.drawInfos, info.drawInfos, DrawInfo);
                    return this;
                }
            };
            exports_16("IndirectBuffer", IndirectBuffer);
            TextureInfo = class TextureInfo {
                constructor(type = TextureType.TEX2D, usage = TextureUsageBit.NONE, format = Format.UNKNOWN, width = 0, height = 0, flags = TextureFlagBit.NONE, layerCount = 1, levelCount = 1, samples = SampleCount.ONE, depth = 1, externalRes = 0) {
                    this.type = type;
                    this.usage = usage;
                    this.format = format;
                    this.width = width;
                    this.height = height;
                    this.flags = flags;
                    this.layerCount = layerCount;
                    this.levelCount = levelCount;
                    this.samples = samples;
                    this.depth = depth;
                    this.externalRes = externalRes;
                }
                copy(info) {
                    this.type = info.type;
                    this.usage = info.usage;
                    this.format = info.format;
                    this.width = info.width;
                    this.height = info.height;
                    this.flags = info.flags;
                    this.layerCount = info.layerCount;
                    this.levelCount = info.levelCount;
                    this.samples = info.samples;
                    this.depth = info.depth;
                    this.externalRes = info.externalRes;
                    return this;
                }
            };
            exports_16("TextureInfo", TextureInfo);
            TextureViewInfo = class TextureViewInfo {
                constructor(texture = null, type = TextureType.TEX2D, format = Format.UNKNOWN, baseLevel = 0, levelCount = 1, baseLayer = 0, layerCount = 1) {
                    this.texture = texture;
                    this.type = type;
                    this.format = format;
                    this.baseLevel = baseLevel;
                    this.levelCount = levelCount;
                    this.baseLayer = baseLayer;
                    this.layerCount = layerCount;
                }
                copy(info) {
                    this.texture = info.texture;
                    this.type = info.type;
                    this.format = info.format;
                    this.baseLevel = info.baseLevel;
                    this.levelCount = info.levelCount;
                    this.baseLayer = info.baseLayer;
                    this.layerCount = info.layerCount;
                    return this;
                }
            };
            exports_16("TextureViewInfo", TextureViewInfo);
            SamplerInfo = class SamplerInfo {
                constructor(minFilter = Filter.LINEAR, magFilter = Filter.LINEAR, mipFilter = Filter.NONE, addressU = Address.WRAP, addressV = Address.WRAP, addressW = Address.WRAP, maxAnisotropy = 0, cmpFunc = ComparisonFunc.ALWAYS) {
                    this.minFilter = minFilter;
                    this.magFilter = magFilter;
                    this.mipFilter = mipFilter;
                    this.addressU = addressU;
                    this.addressV = addressV;
                    this.addressW = addressW;
                    this.maxAnisotropy = maxAnisotropy;
                    this.cmpFunc = cmpFunc;
                }
                copy(info) {
                    this.minFilter = info.minFilter;
                    this.magFilter = info.magFilter;
                    this.mipFilter = info.mipFilter;
                    this.addressU = info.addressU;
                    this.addressV = info.addressV;
                    this.addressW = info.addressW;
                    this.maxAnisotropy = info.maxAnisotropy;
                    this.cmpFunc = info.cmpFunc;
                    return this;
                }
            };
            exports_16("SamplerInfo", SamplerInfo);
            Uniform = class Uniform {
                constructor(name = '', type = Type.UNKNOWN, count = 0) {
                    this.name = name;
                    this.type = type;
                    this.count = count;
                }
                copy(info) {
                    this.name = info.name;
                    this.type = info.type;
                    this.count = info.count;
                    return this;
                }
            };
            exports_16("Uniform", Uniform);
            UniformBlock = class UniformBlock {
                constructor(set = 0, binding = 0, name = '', members = [], count = 0) {
                    this.set = set;
                    this.binding = binding;
                    this.name = name;
                    this.members = members;
                    this.count = count;
                }
                copy(info) {
                    this.set = info.set;
                    this.binding = info.binding;
                    this.name = info.name;
                    deepCopy(this.members, info.members, Uniform);
                    this.count = info.count;
                    return this;
                }
            };
            exports_16("UniformBlock", UniformBlock);
            UniformSamplerTexture = class UniformSamplerTexture {
                constructor(set = 0, binding = 0, name = '', type = Type.UNKNOWN, count = 0) {
                    this.set = set;
                    this.binding = binding;
                    this.name = name;
                    this.type = type;
                    this.count = count;
                }
                copy(info) {
                    this.set = info.set;
                    this.binding = info.binding;
                    this.name = info.name;
                    this.type = info.type;
                    this.count = info.count;
                    return this;
                }
            };
            exports_16("UniformSamplerTexture", UniformSamplerTexture);
            UniformSampler = class UniformSampler {
                constructor(set = 0, binding = 0, name = '', count = 0) {
                    this.set = set;
                    this.binding = binding;
                    this.name = name;
                    this.count = count;
                }
                copy(info) {
                    this.set = info.set;
                    this.binding = info.binding;
                    this.name = info.name;
                    this.count = info.count;
                    return this;
                }
            };
            exports_16("UniformSampler", UniformSampler);
            UniformTexture = class UniformTexture {
                constructor(set = 0, binding = 0, name = '', type = Type.UNKNOWN, count = 0) {
                    this.set = set;
                    this.binding = binding;
                    this.name = name;
                    this.type = type;
                    this.count = count;
                }
                copy(info) {
                    this.set = info.set;
                    this.binding = info.binding;
                    this.name = info.name;
                    this.type = info.type;
                    this.count = info.count;
                    return this;
                }
            };
            exports_16("UniformTexture", UniformTexture);
            UniformStorageImage = class UniformStorageImage {
                constructor(set = 0, binding = 0, name = '', type = Type.UNKNOWN, count = 0, memoryAccess = MemoryAccessBit.READ_WRITE) {
                    this.set = set;
                    this.binding = binding;
                    this.name = name;
                    this.type = type;
                    this.count = count;
                    this.memoryAccess = memoryAccess;
                }
                copy(info) {
                    this.set = info.set;
                    this.binding = info.binding;
                    this.name = info.name;
                    this.type = info.type;
                    this.count = info.count;
                    this.memoryAccess = info.memoryAccess;
                    return this;
                }
            };
            exports_16("UniformStorageImage", UniformStorageImage);
            UniformStorageBuffer = class UniformStorageBuffer {
                constructor(set = 0, binding = 0, name = '', count = 0, memoryAccess = MemoryAccessBit.READ_WRITE) {
                    this.set = set;
                    this.binding = binding;
                    this.name = name;
                    this.count = count;
                    this.memoryAccess = memoryAccess;
                }
                copy(info) {
                    this.set = info.set;
                    this.binding = info.binding;
                    this.name = info.name;
                    this.count = info.count;
                    this.memoryAccess = info.memoryAccess;
                    return this;
                }
            };
            exports_16("UniformStorageBuffer", UniformStorageBuffer);
            UniformInputAttachment = class UniformInputAttachment {
                constructor(set = 0, binding = 0, name = '', count = 0) {
                    this.set = set;
                    this.binding = binding;
                    this.name = name;
                    this.count = count;
                }
                copy(info) {
                    this.set = info.set;
                    this.binding = info.binding;
                    this.name = info.name;
                    this.count = info.count;
                    return this;
                }
            };
            exports_16("UniformInputAttachment", UniformInputAttachment);
            ShaderStage = class ShaderStage {
                constructor(stage = ShaderStageFlagBit.NONE, source = '') {
                    this.stage = stage;
                    this.source = source;
                }
                copy(info) {
                    this.stage = info.stage;
                    this.source = info.source;
                    return this;
                }
            };
            exports_16("ShaderStage", ShaderStage);
            Attribute = class Attribute {
                constructor(name = '', format = Format.UNKNOWN, isNormalized = false, stream = 0, isInstanced = false, location = 0) {
                    this.name = name;
                    this.format = format;
                    this.isNormalized = isNormalized;
                    this.stream = stream;
                    this.isInstanced = isInstanced;
                    this.location = location;
                }
                copy(info) {
                    this.name = info.name;
                    this.format = info.format;
                    this.isNormalized = info.isNormalized;
                    this.stream = info.stream;
                    this.isInstanced = info.isInstanced;
                    this.location = info.location;
                    return this;
                }
            };
            exports_16("Attribute", Attribute);
            ShaderInfo = class ShaderInfo {
                constructor(name = '', stages = [], attributes = [], blocks = [], buffers = [], samplerTextures = [], samplers = [], textures = [], images = [], subpassInputs = []) {
                    this.name = name;
                    this.stages = stages;
                    this.attributes = attributes;
                    this.blocks = blocks;
                    this.buffers = buffers;
                    this.samplerTextures = samplerTextures;
                    this.samplers = samplers;
                    this.textures = textures;
                    this.images = images;
                    this.subpassInputs = subpassInputs;
                }
                copy(info) {
                    this.name = info.name;
                    deepCopy(this.stages, info.stages, ShaderStage);
                    deepCopy(this.attributes, info.attributes, Attribute);
                    deepCopy(this.blocks, info.blocks, UniformBlock);
                    deepCopy(this.buffers, info.buffers, UniformStorageBuffer);
                    deepCopy(this.samplerTextures, info.samplerTextures, UniformSamplerTexture);
                    deepCopy(this.samplers, info.samplers, UniformSampler);
                    deepCopy(this.textures, info.textures, UniformTexture);
                    deepCopy(this.images, info.images, UniformStorageImage);
                    deepCopy(this.subpassInputs, info.subpassInputs, UniformInputAttachment);
                    return this;
                }
            };
            exports_16("ShaderInfo", ShaderInfo);
            InputAssemblerInfo = class InputAssemblerInfo {
                constructor(attributes = [], vertexBuffers = [], indexBuffer = null, indirectBuffer = null) {
                    this.attributes = attributes;
                    this.vertexBuffers = vertexBuffers;
                    this.indexBuffer = indexBuffer;
                    this.indirectBuffer = indirectBuffer;
                }
                copy(info) {
                    deepCopy(this.attributes, info.attributes, Attribute);
                    this.vertexBuffers = info.vertexBuffers.slice();
                    this.indexBuffer = info.indexBuffer;
                    this.indirectBuffer = info.indirectBuffer;
                    return this;
                }
            };
            exports_16("InputAssemblerInfo", InputAssemblerInfo);
            ColorAttachment = class ColorAttachment {
                constructor(format = Format.UNKNOWN, sampleCount = SampleCount.ONE, loadOp = LoadOp.CLEAR, storeOp = StoreOp.STORE, beginAccesses = [], endAccesses = [AccessType.COLOR_ATTACHMENT_WRITE], isGeneralLayout = false) {
                    this.format = format;
                    this.sampleCount = sampleCount;
                    this.loadOp = loadOp;
                    this.storeOp = storeOp;
                    this.beginAccesses = beginAccesses;
                    this.endAccesses = endAccesses;
                    this.isGeneralLayout = isGeneralLayout;
                }
                copy(info) {
                    this.format = info.format;
                    this.sampleCount = info.sampleCount;
                    this.loadOp = info.loadOp;
                    this.storeOp = info.storeOp;
                    this.beginAccesses = info.beginAccesses.slice();
                    this.endAccesses = info.endAccesses.slice();
                    this.isGeneralLayout = info.isGeneralLayout;
                    return this;
                }
            };
            exports_16("ColorAttachment", ColorAttachment);
            DepthStencilAttachment = class DepthStencilAttachment {
                constructor(format = Format.UNKNOWN, sampleCount = SampleCount.ONE, depthLoadOp = LoadOp.CLEAR, depthStoreOp = StoreOp.STORE, stencilLoadOp = LoadOp.CLEAR, stencilStoreOp = StoreOp.STORE, beginAccesses = [], endAccesses = [AccessType.DEPTH_STENCIL_ATTACHMENT_WRITE], isGeneralLayout = false) {
                    this.format = format;
                    this.sampleCount = sampleCount;
                    this.depthLoadOp = depthLoadOp;
                    this.depthStoreOp = depthStoreOp;
                    this.stencilLoadOp = stencilLoadOp;
                    this.stencilStoreOp = stencilStoreOp;
                    this.beginAccesses = beginAccesses;
                    this.endAccesses = endAccesses;
                    this.isGeneralLayout = isGeneralLayout;
                }
                copy(info) {
                    this.format = info.format;
                    this.sampleCount = info.sampleCount;
                    this.depthLoadOp = info.depthLoadOp;
                    this.depthStoreOp = info.depthStoreOp;
                    this.stencilLoadOp = info.stencilLoadOp;
                    this.stencilStoreOp = info.stencilStoreOp;
                    this.beginAccesses = info.beginAccesses.slice();
                    this.endAccesses = info.endAccesses.slice();
                    this.isGeneralLayout = info.isGeneralLayout;
                    return this;
                }
            };
            exports_16("DepthStencilAttachment", DepthStencilAttachment);
            SubpassInfo = class SubpassInfo {
                constructor(inputs = [], colors = [], resolves = [], preserves = [], depthStencil = -1, depthStencilResolve = -1, depthResolveMode = ResolveMode.NONE, stencilResolveMode = ResolveMode.NONE) {
                    this.inputs = inputs;
                    this.colors = colors;
                    this.resolves = resolves;
                    this.preserves = preserves;
                    this.depthStencil = depthStencil;
                    this.depthStencilResolve = depthStencilResolve;
                    this.depthResolveMode = depthResolveMode;
                    this.stencilResolveMode = stencilResolveMode;
                }
                copy(info) {
                    this.inputs = info.inputs.slice();
                    this.colors = info.colors.slice();
                    this.resolves = info.resolves.slice();
                    this.preserves = info.preserves.slice();
                    this.depthStencil = info.depthStencil;
                    this.depthStencilResolve = info.depthStencilResolve;
                    this.depthResolveMode = info.depthResolveMode;
                    this.stencilResolveMode = info.stencilResolveMode;
                    return this;
                }
            };
            exports_16("SubpassInfo", SubpassInfo);
            SubpassDependency = class SubpassDependency {
                constructor(srcSubpass = 0, dstSubpass = 0, srcAccesses = [], dstAccesses = []) {
                    this.srcSubpass = srcSubpass;
                    this.dstSubpass = dstSubpass;
                    this.srcAccesses = srcAccesses;
                    this.dstAccesses = dstAccesses;
                }
                copy(info) {
                    this.srcSubpass = info.srcSubpass;
                    this.dstSubpass = info.dstSubpass;
                    this.srcAccesses = info.srcAccesses.slice();
                    this.dstAccesses = info.dstAccesses.slice();
                    return this;
                }
            };
            exports_16("SubpassDependency", SubpassDependency);
            RenderPassInfo = class RenderPassInfo {
                constructor(colorAttachments = [], depthStencilAttachment = new DepthStencilAttachment(), subpasses = [], dependencies = []) {
                    this.colorAttachments = colorAttachments;
                    this.depthStencilAttachment = depthStencilAttachment;
                    this.subpasses = subpasses;
                    this.dependencies = dependencies;
                }
                copy(info) {
                    deepCopy(this.colorAttachments, info.colorAttachments, ColorAttachment);
                    this.depthStencilAttachment.copy(info.depthStencilAttachment);
                    deepCopy(this.subpasses, info.subpasses, SubpassInfo);
                    deepCopy(this.dependencies, info.dependencies, SubpassDependency);
                    return this;
                }
            };
            exports_16("RenderPassInfo", RenderPassInfo);
            GlobalBarrierInfo = class GlobalBarrierInfo {
                constructor(prevAccesses = [], nextAccesses = []) {
                    this.prevAccesses = prevAccesses;
                    this.nextAccesses = nextAccesses;
                }
                copy(info) {
                    this.prevAccesses = info.prevAccesses.slice();
                    this.nextAccesses = info.nextAccesses.slice();
                    return this;
                }
            };
            exports_16("GlobalBarrierInfo", GlobalBarrierInfo);
            TextureBarrierInfo = class TextureBarrierInfo {
                constructor(prevAccesses = [], nextAccesses = [], discardContents = false, srcQueue = null, dstQueue = null) {
                    this.prevAccesses = prevAccesses;
                    this.nextAccesses = nextAccesses;
                    this.discardContents = discardContents;
                    this.srcQueue = srcQueue;
                    this.dstQueue = dstQueue;
                }
                copy(info) {
                    this.prevAccesses = info.prevAccesses.slice();
                    this.nextAccesses = info.nextAccesses.slice();
                    this.discardContents = info.discardContents;
                    this.srcQueue = info.srcQueue;
                    this.dstQueue = info.dstQueue;
                    return this;
                }
            };
            exports_16("TextureBarrierInfo", TextureBarrierInfo);
            FramebufferInfo = class FramebufferInfo {
                constructor(renderPass = null, colorTextures = [], depthStencilTexture = null) {
                    this.renderPass = renderPass;
                    this.colorTextures = colorTextures;
                    this.depthStencilTexture = depthStencilTexture;
                }
                copy(info) {
                    this.renderPass = info.renderPass;
                    this.colorTextures = info.colorTextures.slice();
                    this.depthStencilTexture = info.depthStencilTexture;
                    return this;
                }
            };
            exports_16("FramebufferInfo", FramebufferInfo);
            DescriptorSetLayoutBinding = class DescriptorSetLayoutBinding {
                constructor(binding = -1, descriptorType = DescriptorType.UNKNOWN, count = 0, stageFlags = ShaderStageFlagBit.NONE, immutableSamplers = []) {
                    this.binding = binding;
                    this.descriptorType = descriptorType;
                    this.count = count;
                    this.stageFlags = stageFlags;
                    this.immutableSamplers = immutableSamplers;
                }
                copy(info) {
                    this.binding = info.binding;
                    this.descriptorType = info.descriptorType;
                    this.count = info.count;
                    this.stageFlags = info.stageFlags;
                    this.immutableSamplers = info.immutableSamplers.slice();
                    return this;
                }
            };
            exports_16("DescriptorSetLayoutBinding", DescriptorSetLayoutBinding);
            DescriptorSetLayoutInfo = class DescriptorSetLayoutInfo {
                constructor(bindings = []) {
                    this.bindings = bindings;
                }
                copy(info) {
                    deepCopy(this.bindings, info.bindings, DescriptorSetLayoutBinding);
                    return this;
                }
            };
            exports_16("DescriptorSetLayoutInfo", DescriptorSetLayoutInfo);
            DescriptorSetInfo = class DescriptorSetInfo {
                constructor(layout = null) {
                    this.layout = layout;
                }
                copy(info) {
                    this.layout = info.layout;
                    return this;
                }
            };
            exports_16("DescriptorSetInfo", DescriptorSetInfo);
            PipelineLayoutInfo = class PipelineLayoutInfo {
                constructor(setLayouts = []) {
                    this.setLayouts = setLayouts;
                }
                copy(info) {
                    this.setLayouts = info.setLayouts.slice();
                    return this;
                }
            };
            exports_16("PipelineLayoutInfo", PipelineLayoutInfo);
            InputState = class InputState {
                constructor(attributes = []) {
                    this.attributes = attributes;
                }
                copy(info) {
                    deepCopy(this.attributes, info.attributes, Attribute);
                    return this;
                }
            };
            exports_16("InputState", InputState);
            CommandBufferInfo = class CommandBufferInfo {
                constructor(queue = null, type = CommandBufferType.PRIMARY) {
                    this.queue = queue;
                    this.type = type;
                }
                copy(info) {
                    this.queue = info.queue;
                    this.type = info.type;
                    return this;
                }
            };
            exports_16("CommandBufferInfo", CommandBufferInfo);
            QueueInfo = class QueueInfo {
                constructor(type = QueueType.GRAPHICS) {
                    this.type = type;
                }
                copy(info) {
                    this.type = info.type;
                    return this;
                }
            };
            exports_16("QueueInfo", QueueInfo);
            FormatInfo = class FormatInfo {
                constructor(name = '', size = 0, count = 0, type = FormatType.NONE, hasAlpha = false, hasDepth = false, hasStencil = false, isCompressed = false) {
                    this.name = name;
                    this.size = size;
                    this.count = count;
                    this.type = type;
                    this.hasAlpha = hasAlpha;
                    this.hasDepth = hasDepth;
                    this.hasStencil = hasStencil;
                    this.isCompressed = isCompressed;
                }
            };
            exports_16("FormatInfo", FormatInfo);
            MemoryStatus = class MemoryStatus {
                constructor(bufferSize = 0, textureSize = 0) {
                    this.bufferSize = bufferSize;
                    this.textureSize = textureSize;
                }
                copy(info) {
                    this.bufferSize = info.bufferSize;
                    this.textureSize = info.textureSize;
                    return this;
                }
            };
            exports_16("MemoryStatus", MemoryStatus);
            DynamicStencilStates = class DynamicStencilStates {
                constructor(writeMask = 0, compareMask = 0, reference = 0) {
                    this.writeMask = writeMask;
                    this.compareMask = compareMask;
                    this.reference = reference;
                }
                copy(info) {
                    this.writeMask = info.writeMask;
                    this.compareMask = info.compareMask;
                    this.reference = info.reference;
                    return this;
                }
            };
            exports_16("DynamicStencilStates", DynamicStencilStates);
            DynamicStates = class DynamicStates {
                constructor(viewport = new Viewport(), scissor = new Rect(), blendConstant = new Color(), lineWidth = 1, depthBiasConstant = 0, depthBiasClamp = 0, depthBiasSlope = 0, depthMinBounds = 0, depthMaxBounds = 0, stencilStatesFront = new DynamicStencilStates(), stencilStatesBack = new DynamicStencilStates()) {
                    this.viewport = viewport;
                    this.scissor = scissor;
                    this.blendConstant = blendConstant;
                    this.lineWidth = lineWidth;
                    this.depthBiasConstant = depthBiasConstant;
                    this.depthBiasClamp = depthBiasClamp;
                    this.depthBiasSlope = depthBiasSlope;
                    this.depthMinBounds = depthMinBounds;
                    this.depthMaxBounds = depthMaxBounds;
                    this.stencilStatesFront = stencilStatesFront;
                    this.stencilStatesBack = stencilStatesBack;
                }
                copy(info) {
                    this.viewport.copy(info.viewport);
                    this.scissor.copy(info.scissor);
                    this.blendConstant.copy(info.blendConstant);
                    this.lineWidth = info.lineWidth;
                    this.depthBiasConstant = info.depthBiasConstant;
                    this.depthBiasClamp = info.depthBiasClamp;
                    this.depthBiasSlope = info.depthBiasSlope;
                    this.depthMinBounds = info.depthMinBounds;
                    this.depthMaxBounds = info.depthMaxBounds;
                    this.stencilStatesFront.copy(info.stencilStatesFront);
                    this.stencilStatesBack.copy(info.stencilStatesBack);
                    return this;
                }
            };
            exports_16("DynamicStates", DynamicStates);
            /**
             * ========================= !DO NOT CHANGE THE ABOVE SECTION MANUALLY! =========================
             * The above section is auto-generated from engine-native/cocos/renderer/core/gfx/GFXDef-common.h
             * by the script engine-native/tools/gfx-define-generator/generate.js.
             * Changes to these public interfaces should be made there first and synced back.
             * ========================= !DO NOT CHANGE THE ABOVE SECTION MANUALLY! =========================
             */
            /**
             * @en GFX base object.
             * @zh GFX 基类对象。
             */
            GFXObject = class GFXObject {
                constructor(objectType) {
                    this._objectType = ObjectType.UNKNOWN;
                    this._objectID = 0;
                    this._typedID = 0;
                    this._objectType = objectType;
                    this._objectID = GFXObject._idTable[ObjectType.UNKNOWN]++;
                    this._typedID = GFXObject._idTable[objectType]++;
                }
                get objectType() {
                    return this._objectType;
                }
                get objectID() {
                    return this._objectID;
                }
                get typedID() {
                    return this._typedID;
                }
            };
            exports_16("GFXObject", GFXObject);
            GFXObject._idTable = Array(ObjectType.COUNT).fill(1 << 16);
            (function (AttributeName) {
                AttributeName["ATTR_POSITION"] = "a_position";
                AttributeName["ATTR_NORMAL"] = "a_normal";
                AttributeName["ATTR_TANGENT"] = "a_tangent";
                AttributeName["ATTR_BITANGENT"] = "a_bitangent";
                AttributeName["ATTR_WEIGHTS"] = "a_weights";
                AttributeName["ATTR_JOINTS"] = "a_joints";
                AttributeName["ATTR_COLOR"] = "a_color";
                AttributeName["ATTR_COLOR1"] = "a_color1";
                AttributeName["ATTR_COLOR2"] = "a_color2";
                AttributeName["ATTR_TEX_COORD"] = "a_texCoord";
                AttributeName["ATTR_TEX_COORD1"] = "a_texCoord1";
                AttributeName["ATTR_TEX_COORD2"] = "a_texCoord2";
                AttributeName["ATTR_TEX_COORD3"] = "a_texCoord3";
                AttributeName["ATTR_TEX_COORD4"] = "a_texCoord4";
                AttributeName["ATTR_TEX_COORD5"] = "a_texCoord5";
                AttributeName["ATTR_TEX_COORD6"] = "a_texCoord6";
                AttributeName["ATTR_TEX_COORD7"] = "a_texCoord7";
                AttributeName["ATTR_TEX_COORD8"] = "a_texCoord8";
                AttributeName["ATTR_BATCH_ID"] = "a_batch_id";
                AttributeName["ATTR_BATCH_UV"] = "a_batch_uv";
            })(AttributeName || (AttributeName = {}));
            exports_16("AttributeName", AttributeName);
            exports_16("FormatInfos", FormatInfos = Object.freeze([
                new FormatInfo('UNKNOWN', 0, 0, FormatType.NONE, false, false, false, false),
                new FormatInfo('A8', 1, 1, FormatType.UNORM, true, false, false, false),
                new FormatInfo('L8', 1, 1, FormatType.UNORM, false, false, false, false),
                new FormatInfo('LA8', 1, 2, FormatType.UNORM, true, false, false, false),
                new FormatInfo('R8', 1, 1, FormatType.UNORM, false, false, false, false),
                new FormatInfo('R8SN', 1, 1, FormatType.SNORM, false, false, false, false),
                new FormatInfo('R8UI', 1, 1, FormatType.UINT, false, false, false, false),
                new FormatInfo('R8I', 1, 1, FormatType.INT, false, false, false, false),
                new FormatInfo('R16F', 2, 1, FormatType.FLOAT, false, false, false, false),
                new FormatInfo('R16UI', 2, 1, FormatType.UINT, false, false, false, false),
                new FormatInfo('R16I', 2, 1, FormatType.INT, false, false, false, false),
                new FormatInfo('R32F', 4, 1, FormatType.FLOAT, false, false, false, false),
                new FormatInfo('R32UI', 4, 1, FormatType.UINT, false, false, false, false),
                new FormatInfo('R32I', 4, 1, FormatType.INT, false, false, false, false),
                new FormatInfo('RG8', 2, 2, FormatType.UNORM, false, false, false, false),
                new FormatInfo('RG8SN', 2, 2, FormatType.SNORM, false, false, false, false),
                new FormatInfo('RG8UI', 2, 2, FormatType.UINT, false, false, false, false),
                new FormatInfo('RG8I', 2, 2, FormatType.INT, false, false, false, false),
                new FormatInfo('RG16F', 4, 2, FormatType.FLOAT, false, false, false, false),
                new FormatInfo('RG16UI', 4, 2, FormatType.UINT, false, false, false, false),
                new FormatInfo('RG16I', 4, 2, FormatType.INT, false, false, false, false),
                new FormatInfo('RG32F', 8, 2, FormatType.FLOAT, false, false, false, false),
                new FormatInfo('RG32UI', 8, 2, FormatType.UINT, false, false, false, false),
                new FormatInfo('RG32I', 8, 2, FormatType.INT, false, false, false, false),
                new FormatInfo('RGB8', 3, 3, FormatType.UNORM, false, false, false, false),
                new FormatInfo('SRGB8', 3, 3, FormatType.UNORM, false, false, false, false),
                new FormatInfo('RGB8SN', 3, 3, FormatType.SNORM, false, false, false, false),
                new FormatInfo('RGB8UI', 3, 3, FormatType.UINT, false, false, false, false),
                new FormatInfo('RGB8I', 3, 3, FormatType.INT, false, false, false, false),
                new FormatInfo('RGB16F', 6, 3, FormatType.FLOAT, false, false, false, false),
                new FormatInfo('RGB16UI', 6, 3, FormatType.UINT, false, false, false, false),
                new FormatInfo('RGB16I', 6, 3, FormatType.INT, false, false, false, false),
                new FormatInfo('RGB32F', 12, 3, FormatType.FLOAT, false, false, false, false),
                new FormatInfo('RGB32UI', 12, 3, FormatType.UINT, false, false, false, false),
                new FormatInfo('RGB32I', 12, 3, FormatType.INT, false, false, false, false),
                new FormatInfo('RGBA8', 4, 4, FormatType.UNORM, true, false, false, false),
                new FormatInfo('BGRA8', 4, 4, FormatType.UNORM, true, false, false, false),
                new FormatInfo('SRGB8_A8', 4, 4, FormatType.UNORM, true, false, false, false),
                new FormatInfo('RGBA8SN', 4, 4, FormatType.SNORM, true, false, false, false),
                new FormatInfo('RGBA8UI', 4, 4, FormatType.UINT, true, false, false, false),
                new FormatInfo('RGBA8I', 4, 4, FormatType.INT, true, false, false, false),
                new FormatInfo('RGBA16F', 8, 4, FormatType.FLOAT, true, false, false, false),
                new FormatInfo('RGBA16UI', 8, 4, FormatType.UINT, true, false, false, false),
                new FormatInfo('RGBA16I', 8, 4, FormatType.INT, true, false, false, false),
                new FormatInfo('RGBA32F', 16, 4, FormatType.FLOAT, true, false, false, false),
                new FormatInfo('RGBA32UI', 16, 4, FormatType.UINT, true, false, false, false),
                new FormatInfo('RGBA32I', 16, 4, FormatType.INT, true, false, false, false),
                new FormatInfo('R5G6B5', 2, 3, FormatType.UNORM, false, false, false, false),
                new FormatInfo('R11G11B10F', 4, 3, FormatType.FLOAT, false, false, false, false),
                new FormatInfo('RGB5A1', 2, 4, FormatType.UNORM, true, false, false, false),
                new FormatInfo('RGBA4', 2, 4, FormatType.UNORM, true, false, false, false),
                new FormatInfo('RGB10A2', 2, 4, FormatType.UNORM, true, false, false, false),
                new FormatInfo('RGB10A2UI', 2, 4, FormatType.UINT, true, false, false, false),
                new FormatInfo('RGB9E5', 2, 4, FormatType.FLOAT, true, false, false, false),
                new FormatInfo('DEPTH', 4, 1, FormatType.FLOAT, false, true, false, false),
                new FormatInfo('DEPTH_STENCIL', 5, 2, FormatType.FLOAT, false, true, true, false),
                new FormatInfo('BC1', 1, 3, FormatType.UNORM, false, false, false, true),
                new FormatInfo('BC1_ALPHA', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('BC1_SRGB', 1, 3, FormatType.UNORM, false, false, false, true),
                new FormatInfo('BC1_SRGB_ALPHA', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('BC2', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('BC2_SRGB', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('BC3', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('BC3_SRGB', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('BC4', 1, 1, FormatType.UNORM, false, false, false, true),
                new FormatInfo('BC4_SNORM', 1, 1, FormatType.SNORM, false, false, false, true),
                new FormatInfo('BC5', 1, 2, FormatType.UNORM, false, false, false, true),
                new FormatInfo('BC5_SNORM', 1, 2, FormatType.SNORM, false, false, false, true),
                new FormatInfo('BC6H_UF16', 1, 3, FormatType.UFLOAT, false, false, false, true),
                new FormatInfo('BC6H_SF16', 1, 3, FormatType.FLOAT, false, false, false, true),
                new FormatInfo('BC7', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('BC7_SRGB', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ETC_RGB8', 1, 3, FormatType.UNORM, false, false, false, true),
                new FormatInfo('ETC2_RGB8', 1, 3, FormatType.UNORM, false, false, false, true),
                new FormatInfo('ETC2_SRGB8', 1, 3, FormatType.UNORM, false, false, false, true),
                new FormatInfo('ETC2_RGB8_A1', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ETC2_SRGB8_A1', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ETC2_RGBA8', 2, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ETC2_SRGB8_A8', 2, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('EAC_R11', 1, 1, FormatType.UNORM, false, false, false, true),
                new FormatInfo('EAC_R11SN', 1, 1, FormatType.SNORM, false, false, false, true),
                new FormatInfo('EAC_RG11', 2, 2, FormatType.UNORM, false, false, false, true),
                new FormatInfo('EAC_RG11SN', 2, 2, FormatType.SNORM, false, false, false, true),
                new FormatInfo('PVRTC_RGB2', 2, 3, FormatType.UNORM, false, false, false, true),
                new FormatInfo('PVRTC_RGBA2', 2, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('PVRTC_RGB4', 2, 3, FormatType.UNORM, false, false, false, true),
                new FormatInfo('PVRTC_RGBA4', 2, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('PVRTC2_2BPP', 2, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('PVRTC2_4BPP', 2, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_4x4', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_5x4', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_5x5', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_6x5', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_6x6', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_8x5', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_8x6', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_8x8', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_10x5', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_10x6', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_10x8', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_10x10', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_12x10', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_RGBA_12x12', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_4x4', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_5x4', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_5x5', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_6x5', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_6x6', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_8x5', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_8x6', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_8x8', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_10x5', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_10x6', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_10x8', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_10x10', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_12x10', 1, 4, FormatType.UNORM, true, false, false, true),
                new FormatInfo('ASTC_SRGBA_12x12', 1, 4, FormatType.UNORM, true, false, false, true),
            ]));
            exports_16("DESCRIPTOR_BUFFER_TYPE", DESCRIPTOR_BUFFER_TYPE = DescriptorType.UNIFORM_BUFFER | DescriptorType.DYNAMIC_UNIFORM_BUFFER
                | DescriptorType.STORAGE_BUFFER | DescriptorType.DYNAMIC_STORAGE_BUFFER);
            exports_16("DESCRIPTOR_SAMPLER_TYPE", DESCRIPTOR_SAMPLER_TYPE = DescriptorType.SAMPLER_TEXTURE | DescriptorType.SAMPLER | DescriptorType.TEXTURE
                | DescriptorType.STORAGE_IMAGE | DescriptorType.INPUT_ATTACHMENT);
            exports_16("DESCRIPTOR_DYNAMIC_TYPE", DESCRIPTOR_DYNAMIC_TYPE = DescriptorType.DYNAMIC_STORAGE_BUFFER | DescriptorType.DYNAMIC_UNIFORM_BUFFER);
            exports_16("DRAW_INFO_SIZE", DRAW_INFO_SIZE = 28);
            _type2size = [
                0,
                4,
                8,
                12,
                16,
                4,
                8,
                12,
                16,
                4,
                8,
                12,
                16,
                4,
                8,
                12,
                16,
                16,
                24,
                32,
                24,
                36,
                48,
                32,
                48,
                64,
                4,
                4,
                4,
                4,
                4,
                4, // SAMPLER_CUBE
            ];
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/buffer", ["engine/cocos/core/gfx/base/define"], function (exports_17, context_17) {
    "use strict";
    var define_15, Buffer;
    var __moduleName = context_17 && context_17.id;
    return {
        setters: [
            function (define_15_1) {
                define_15 = define_15_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX buffer.
             * @zh GFX 缓冲。
             */
            Buffer = class Buffer extends define_15.GFXObject {
                constructor() {
                    super(define_15.ObjectType.BUFFER);
                    this._usage = define_15.BufferUsageBit.NONE;
                    this._memUsage = define_15.MemoryUsageBit.NONE;
                    this._size = 0;
                    this._stride = 1;
                    this._count = 0;
                    this._flags = define_15.BufferFlagBit.NONE;
                    this._isBufferView = false;
                }
                /**
                 * @en Usage type of the buffer.
                 * @zh 缓冲使用方式。
                 */
                get usage() {
                    return this._usage;
                }
                /**
                 * @en Memory usage of the buffer.
                 * @zh 缓冲的内存使用方式。
                 */
                get memUsage() {
                    return this._memUsage;
                }
                /**
                 * @en Size of the buffer.
                 * @zh 缓冲大小。
                 */
                get size() {
                    return this._size;
                }
                /**
                 * @en Stride of the buffer.
                 * @zh 缓冲步长。
                 */
                get stride() {
                    return this._stride;
                }
                /**
                 * @en Count of the buffer wrt. stride.
                 * @zh 缓冲条目数量。
                 */
                get count() {
                    return this._count;
                }
                get flags() {
                    return this._flags;
                }
            };
            exports_17("Buffer", Buffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/descriptor-set", ["engine/cocos/core/gfx/base/define"], function (exports_18, context_18) {
    "use strict";
    var define_16, DescriptorSet;
    var __moduleName = context_18 && context_18.id;
    return {
        setters: [
            function (define_16_1) {
                define_16 = define_16_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX descriptor sets.
             * @zh GFX 描述符集组。
             */
            DescriptorSet = class DescriptorSet extends define_16.GFXObject {
                constructor() {
                    super(define_16.ObjectType.DESCRIPTOR_SET);
                    this._layout = null;
                    this._buffers = [];
                    this._textures = [];
                    this._samplers = [];
                    this._isDirty = false;
                }
                get layout() {
                    return this._layout;
                }
                /**
                 * @en Bind buffer to the specified descriptor.
                 * @zh 在指定的描述符位置上绑定缓冲。
                 * @param binding The target binding.
                 * @param buffer The buffer to be bound.
                 */
                bindBuffer(binding, buffer, index = 0) {
                    const bindingIndex = this._layout.bindingIndices[binding];
                    const info = this._layout.bindings[bindingIndex];
                    if (!info) {
                        return;
                    }
                    if (info.descriptorType & define_16.DESCRIPTOR_BUFFER_TYPE) {
                        const descriptorIndex = this._layout.descriptorIndices[binding];
                        if (this._buffers[descriptorIndex + index] !== buffer) {
                            this._buffers[descriptorIndex + index] = buffer;
                            this._isDirty = true;
                        }
                    }
                }
                /**
                 * @en Bind sampler to the specified descriptor.
                 * @zh 在指定的描述符位置上绑定采样器。
                 * @param binding The target binding.
                 * @param sampler The sampler to be bound.
                 */
                bindSampler(binding, sampler, index = 0) {
                    const bindingIndex = this._layout.bindingIndices[binding];
                    const info = this._layout.bindings[bindingIndex];
                    if (!info) {
                        return;
                    }
                    if (info.descriptorType & define_16.DESCRIPTOR_SAMPLER_TYPE) {
                        const descriptorIndex = this._layout.descriptorIndices[binding];
                        if (this._samplers[descriptorIndex + index] !== sampler) {
                            this._samplers[descriptorIndex + index] = sampler;
                            this._isDirty = true;
                        }
                    }
                }
                /**
                 * @en Bind texture to the specified descriptor.
                 * @zh 在指定的描述符位置上绑定纹理。
                 * @param binding The target binding.
                 * @param texture The texture to be bound.
                 */
                bindTexture(binding, texture, index = 0) {
                    const bindingIndex = this._layout.bindingIndices[binding];
                    const info = this._layout.bindings[bindingIndex];
                    if (!info) {
                        return;
                    }
                    if (info.descriptorType & define_16.DESCRIPTOR_SAMPLER_TYPE) {
                        const descriptorIndex = this._layout.descriptorIndices[binding];
                        if (this._textures[descriptorIndex + index] !== texture) {
                            this._textures[descriptorIndex + index] = texture;
                            this._isDirty = true;
                        }
                    }
                }
                /**
                 * @en Get buffer from the specified binding location.
                 * @zh 获取当前指定绑定位置上的缓冲。
                 * @param binding The target binding.
                 */
                getBuffer(binding, index = 0) {
                    const descriptorIndex = this._layout.descriptorIndices[binding];
                    return this._buffers[descriptorIndex + index];
                }
                /**
                 * @en Get sampler from the specified binding location.
                 * @zh 获取当前指定绑定位置上的采样器。
                 * @param binding The target binding.
                 */
                getSampler(binding, index = 0) {
                    const descriptorIndex = this._layout.descriptorIndices[binding];
                    return this._samplers[descriptorIndex + index];
                }
                /**
                 * @en Get texture from the specified binding location.
                 * @zh 获取当前指定绑定位置上的贴图。
                 * @param binding The target binding.
                 */
                getTexture(binding, index = 0) {
                    const descriptorIndex = this._layout.descriptorIndices[binding];
                    return this._textures[descriptorIndex + index];
                }
            };
            exports_18("DescriptorSet", DescriptorSet);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/base/device", ["engine/cocos/core/gfx/base/define"], function (exports_19, context_19) {
    "use strict";
    var define_17, Device;
    var __moduleName = context_19 && context_19.id;
    return {
        setters: [
            function (define_17_1) {
                define_17 = define_17_1;
            }
        ],
        execute: function () {
            /**
             * @en GFX Device.
             * @zh GFX 设备。
             */
            Device = class Device {
                constructor() {
                    this._gfxAPI = define_17.API.UNKNOWN;
                    this._renderer = '';
                    this._vendor = '';
                    this._features = new Array(define_17.Feature.COUNT);
                    this._queue = null;
                    this._cmdBuff = null;
                    this._numDrawCalls = 0;
                    this._numInstances = 0;
                    this._numTris = 0;
                    this._memoryStatus = new define_17.MemoryStatus();
                    this._caps = new define_17.DeviceCaps();
                    this._bindingMappingInfo = new define_17.BindingMappingInfo();
                    this._samplers = new Map();
                    this._globalBarriers = new Map();
                    this._textureBarriers = new Map();
                }
                /**
                 * @en Current rendering API.
                 * @zh 当前 GFX 使用的渲染 API。
                 */
                get gfxAPI() {
                    return this._gfxAPI;
                }
                /**
                 * @en GFX default queue.
                 * @zh GFX 默认队列。
                 */
                get queue() {
                    return this._queue;
                }
                /**
                 * @en GFX default command buffer.
                 * @zh GFX 默认命令缓冲。
                 */
                get commandBuffer() {
                    return this._cmdBuff;
                }
                /**
                 * @en Renderer description.
                 * @zh 渲染器描述。
                 */
                get renderer() {
                    return this._renderer;
                }
                /**
                 * @en Vendor description.
                 * @zh 厂商描述。
                 */
                get vendor() {
                    return this._vendor;
                }
                /**
                 * @en Number of draw calls currently recorded.
                 * @zh 绘制调用次数。
                 */
                get numDrawCalls() {
                    return this._numDrawCalls;
                }
                /**
                 * @en Number of instances currently recorded.
                 * @zh 绘制 Instance 数量。
                 */
                get numInstances() {
                    return this._numInstances;
                }
                /**
                 * @en Number of triangles currently recorded.
                 * @zh 渲染三角形数量。
                 */
                get numTris() {
                    return this._numTris;
                }
                /**
                 * @en Total memory size currently allocated.
                 * @zh 内存状态。
                 */
                get memoryStatus() {
                    return this._memoryStatus;
                }
                /**
                 * @en Current device capabilities.
                 * @zh 当前设备能力数据。
                 */
                get capabilities() {
                    return this._caps;
                }
                /**
                 * @en Current device binding mappings.
                 * @zh 当前设备的绑定槽位映射关系。
                 */
                get bindingMappingInfo() {
                    return this._bindingMappingInfo;
                }
                /**
                 * @en Whether the device has specific feature.
                 * @zh 是否具备特性。
                 * @param feature The GFX feature to be queried.
                 */
                hasFeature(feature) {
                    return this._features[feature];
                }
            };
            exports_19("Device", Device);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/memop/cached-array", [], function (exports_20, context_20) {
    "use strict";
    var CachedArray;
    var __moduleName = context_20 && context_20.id;
    return {
        setters: [],
        execute: function () {
            /**
             * @packageDocumentation
             * @module memop
             */
            /**
             * @en
             * Cached array is a data structure for objects cache, it's designed for persistent data.
             * Its content array length will keep grow.
             * @zh
             * 适用于对象缓存的数组类型封装，一般用于不易被移除的常驻数据。
             * 它的内部数组长度会持续增长，不会减少。
             */
            CachedArray = class CachedArray {
                /**
                 * @param length Initial length
                 * @param compareFn Comparison function for sorting
                 */
                constructor(length, compareFn) {
                    /**
                     * @en
                     * The actual count of data object
                     * @zh
                     * 实际数据内容数量
                     */
                    this.length = 0;
                    this.array = new Array(length);
                    this.length = 0;
                    this._compareFn = compareFn;
                }
                /**
                 * @en
                 * Push an element to the end of the array
                 * @zh
                 * 向数组末尾添加一个元素
                 * @param item The item to be added
                 */
                push(item) {
                    this.array[this.length++] = item;
                }
                /**
                 * @en
                 * Pop the last element in the array. The [[length]] will reduce, but the internal array will keep its size.
                 * @zh
                 * 弹出数组最后一个元素，CachedArray 的 [[length]] 会减少，但是内部数组的实际长度不变
                 * @return The last element.
                 */
                pop() {
                    return this.array[--this.length];
                }
                /**
                 * @en
                 * Get the element at the specified index of the array
                 * @zh
                 * 得到数组中指定位置的元素
                 * @param idx The index of the requested element
                 * @return The element at given index
                 */
                get(idx) {
                    return this.array[idx];
                }
                /**
                 * @en
                 * Clear the cache. The [[length]] will be set to 0, but the internal array will keep its size.
                 * @zh
                 * 清空数组所有元素。[[length]] 会被设为 0，但内部数组的实际长度不变
                 */
                clear() {
                    this.length = 0;
                }
                /**
                 * @en
                 * Clear the cache. The [[length]] will be set to 0, and clear the internal array.
                 * @zh
                 * 清空数组所有元素。[[length]] 会被设为 0，并且清空内部数组
                 */
                destroy() {
                    this.length = 0;
                    this.array.length = 0;
                }
                /**
                 * @en
                 * Sort the existing elements in cache
                 * @zh
                 * 排序所有现有元素
                 */
                sort() {
                    this.array.length = this.length;
                    this.array.sort(this._compareFn);
                }
                /**
                 * @en
                 * Add all elements of a given array to the end of the current array
                 * @zh
                 * 添加一个指定数组中的所有元素到当前数组末尾
                 * @param array The given array to be appended
                 */
                concat(array) {
                    for (let i = 0; i < array.length; ++i) {
                        this.array[this.length++] = array[i];
                    }
                }
                /**
                 * @en Delete the element at the specified location and move the last element to that location.
                 * @zh 删除指定位置的元素并将最后一个元素移动至该位置。
                 * @param idx The index of the element to be deleted
                 */
                fastRemove(idx) {
                    if (idx >= this.length || idx < 0) {
                        return;
                    }
                    const last = --this.length;
                    this.array[idx] = this.array[last];
                }
                /**
                 * @en Returns the first index at which a given element can be found in the array.
                 * @zh 返回在数组中可以找到一个给定元素的第一个索引。
                 * @param val The element
                 */
                indexOf(val) {
                    for (let i = 0, len = this.length; i < len; ++i) {
                        if (this.array[i] === val) {
                            return i;
                        }
                    }
                    return -1;
                }
            };
            exports_20("CachedArray", CachedArray);
        }
    };
});
/*
 Copyright (c) 2017-2020 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/global-exports", ["internal:constants"], function (exports_21, context_21) {
    "use strict";
    var internal_constants_1, _global, legacyCC, engineVersion;
    var __moduleName = context_21 && context_21.id;
    return {
        setters: [
            function (internal_constants_1_1) {
                internal_constants_1 = internal_constants_1_1;
            }
        ],
        execute: function () {
            _global = typeof window === 'undefined' ? global : window;
            /**
             * @en
             * The main namespace of Cocos2d-JS, all engine core classes, functions, properties and constants are defined in this namespace.
             * @zh
             * Cocos 引擎的主要命名空间，引擎代码中所有的类，函数，属性和常量都在这个命名空间中定义。
             * @deprecated
             */
            exports_21("legacyCC", legacyCC = {
                _global,
            });
            // For internal usage
            legacyCC.internal = {};
            if (internal_constants_1.EXPORT_TO_GLOBAL) {
                // Supports dynamically access from external scripts such as adapters and debugger.
                // So macros should still defined in global even if inlined in engine.
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_BUILD (Available for built package)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_BUILD（在构建后生效）
                 */
                _global.CC_BUILD = internal_constants_1.BUILD;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_TEST (Available for ci test environment)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_TEST（在 CI 测试环境下生效）
                 */
                _global.CC_TEST = internal_constants_1.TEST;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_EDITOR (Available for editor environment)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_EDITOR（在编辑器环境下生效）
                 */
                _global.CC_EDITOR = internal_constants_1.EDITOR;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_PREVIEW (Available for preview)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_PREVIEW（预览时生效）
                 */
                _global.CC_PREVIEW = internal_constants_1.PREVIEW;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_DEV (Internal)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_DEV（内部使用）
                 */
                _global.CC_DEV = internal_constants_1.DEV;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_DEBUG (Available for debug environment)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_DEBUG（在调试模式下生效）
                 */
                _global.CC_DEBUG = internal_constants_1.DEBUG;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_JSB (Available for native application environment)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_JSB（在原生应用环境下生效）
                 */
                _global.CC_JSB = internal_constants_1.JSB;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_BYTEDANCE (Available for Bytedance platform)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_BYTEDANCE（在字节平台上生效）
                 */
                _global.CC_BYTEDANCE = internal_constants_1.BYTEDANCE;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_WECHAT (Available for Wechat mini game platform)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_WECHAT（在微信小游戏平台上生效）
                 */
                _global.CC_WECHAT = internal_constants_1.WECHAT;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_ALIPAY (Available for Alipay mini game platform)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_ALIPAY（在支付宝小游戏平台上生效）
                 */
                _global.CC_ALIPAY = internal_constants_1.ALIPAY;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_XIAOMI (Available for MI mini game platform)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_XIAOMI（在小米小游戏平台上生效）
                 */
                _global.CC_XIAOMI = internal_constants_1.XIAOMI;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_BAIDU (Available for Baidu mini game platform)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_BAIDU（在百度小游戏平台上生效）
                 */
                _global.CC_BAIDU = internal_constants_1.BAIDU;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_COCOSPLAY (Available for Cocos Play platform)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_COCOSPLAY（在 CocosPlay 小游戏平台上生效）
                 */
                _global.CC_COCOSPLAY = internal_constants_1.COCOSPLAY;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_HUAWEI (Available for Huawei mini game platform)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_HUAWEI（在华为快游戏平台上生效）
                 */
                _global.CC_HUAWEI = internal_constants_1.HUAWEI;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_OPPO (Available for OPPO mini game platform)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_OPPO（在 OPPO 小游戏平台上生效）
                 */
                _global.CC_OPPO = internal_constants_1.OPPO;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_VIVO (Available for Vivo mini game platform)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_VIVO（在 Vivo 小游戏平台上生效）
                 */
                _global.CC_VIVO = internal_constants_1.VIVO;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_MINIGAME (Available for general mini game platforms)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_MINIGAME（在各个小游戏平台上生效）
                 */
                _global.CC_MINIGAME = internal_constants_1.MINIGAME;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_RUNTIME_BASED (Available for Huawei, OPPO, Vivo and Cocos Play)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_RUNTIME_BASED（在华为、OPPO、Vivo 和 CocosPlay 平台上生效）
                 */
                _global.CC_RUNTIME_BASED = internal_constants_1.RUNTIME_BASED;
                /**
                 * @en The pre-compilation constant for code tree shaking: CC_SUPPORT_JIT (Available for platforms support JIT)
                 * @zh 预编译宏变量，通常用来做平台或环境相关自动代码剔除：CC_SUPPORT_JIT（在支持 JIT 的平台上生效）
                 */
                _global.CC_SUPPORT_JIT = internal_constants_1.SUPPORT_JIT;
                /**
                 * @zh 预编译宏变量，决定是否使用 GPUDriven
                 */
                _global.CC_UI_GPU_DRIVEN = internal_constants_1.UI_GPU_DRIVEN;
            }
            if (internal_constants_1.DEV) {
                legacyCC._Test = {};
            }
            engineVersion = '3.4.0';
            exports_21("VERSION", engineVersion);
            /**
             * The current version of Cocos2d being used.<br/>
             * Please DO NOT remove this String, it is an important flag for bug tracking.<br/>
             * If you post a bug to forum, please attach this flag.
             */
            _global.CocosEngine = legacyCC.ENGINE_VERSION = engineVersion;
            _global.cc = legacyCC;
        }
    };
});
/*
 Copyright (c) 2018-2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
  worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
  not use Cocos Creator software for developing other software or tools that's
  used for developing games. You are not granted to publish, distribute,
  sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/
/**
 * @packageDocumentation
 * @module core
 */
System.register("engine/cocos/core/platform/debug", ["internal:constants", "../../../DebugInfos", "engine/cocos/core/global-exports"], function (exports_22, context_22) {
    "use strict";
    var internal_constants_2, DebugInfos_1, global_exports_1, ERROR_MAP_URL, logList, ccLog, ccWarn, ccError, ccAssert, ccDebug, logFormatter, warnFormatter, errorFormatter, assertFormatter, DebugMode;
    var __moduleName = context_22 && context_22.id;
    function formatString(message, ...optionalParams) {
        // eslint-disable-next-line @typescript-eslint/no-unsafe-return
        return global_exports_1.legacyCC.js.formatStr.apply(null, [message].concat(optionalParams));
    }
    /**
     * @en Outputs a message to the Cocos Creator Console (editor) or Web Console (runtime).
     * @zh 输出一条消息到 Cocos Creator 编辑器的 Console 或运行时 Web 端的 Console 中。
     * @param message - A JavaScript string containing zero or more substitution strings.
     * @param optionalParams - JavaScript objects with which to replace substitution strings within msg.
     * This gives you additional control over the format of the output.
     */
    function log(message, ...optionalParams) {
        return ccLog(message, ...optionalParams);
    }
    exports_22("log", log);
    /**
     * @en
     * Outputs a warning message to the Cocos Creator Console (editor) or Web Console (runtime).
     * - In Cocos Creator, warning is yellow.
     * - In Chrome, warning have a yellow warning icon with the message text.
     * @zh
     * 输出警告消息到 Cocos Creator 编辑器的 Console 或运行时 Web 端的 Console 中。<br/>
     * - 在 Cocos Creator 中，警告信息显示是黄色的。<br/>
     * - 在 Chrome 中，警告信息有着黄色的图标以及黄色的消息文本。<br/>
     * @param message - A JavaScript string containing zero or more substitution strings.
     * @param optionalParams - JavaScript objects with which to replace substitution strings within msg.
     * This gives you additional control over the format of the output.
     */
    function warn(message, ...optionalParams) {
        return ccWarn(message, ...optionalParams);
    }
    exports_22("warn", warn);
    /**
     * @en
     * Outputs an error message to the Cocos Creator Console (editor) or Web Console (runtime).<br/>
     * - In Cocos Creator, error is red.<br/>
     * - In Chrome, error have a red icon along with red message text.<br/>
     * @zh
     * 输出错误消息到 Cocos Creator 编辑器的 Console 或运行时页面端的 Console 中。<br/>
     * - 在 Cocos Creator 中，错误信息显示是红色的。<br/>
     * - 在 Chrome 中，错误信息有红色的图标以及红色的消息文本。<br/>
     * @param message - A JavaScript string containing zero or more substitution strings.
     * @param optionalParams - JavaScript objects with which to replace substitution strings within msg.
     * This gives you additional control over the format of the output.
     */
    function error(message, ...optionalParams) {
        return ccError(message, ...optionalParams);
    }
    exports_22("error", error);
    /**
     * @en
     * Assert the condition and output error messages if the condition is not true.
     * @zh
     * 对检查测试条件进行检查，如果条件不为 true 则输出错误消息
     * @param value - The condition to check on
     * @param message - A JavaScript string containing zero or more substitution strings.
     * @param optionalParams - JavaScript objects with which to replace substitution strings within msg.
     * This gives you additional control over the format of the output.
     */
    function assert(value, message, ...optionalParams) {
        return ccAssert(value, message, ...optionalParams);
    }
    exports_22("assert", assert);
    /**
     * @en Outputs a message at the "debug" log level.
     * @zh 输出一条“调试”日志等级的消息。
     */
    function debug(...data) {
        return ccDebug(...data);
    }
    exports_22("debug", debug);
    function _resetDebugSetting(mode) {
        // reset
        ccLog = ccWarn = ccError = ccAssert = ccDebug = () => {
        };
        if (mode === DebugMode.NONE) {
            return;
        }
        if (mode > DebugMode.ERROR) {
            // Log to web page.
            const logToWebPage = (msg) => {
                if (!global_exports_1.legacyCC.game.canvas) {
                    return;
                }
                if (!logList) {
                    const logDiv = document.createElement('Div');
                    logDiv.setAttribute('id', 'logInfoDiv');
                    logDiv.setAttribute('width', '200');
                    logDiv.setAttribute('height', global_exports_1.legacyCC.game.canvas.height);
                    const logDivStyle = logDiv.style;
                    logDivStyle.zIndex = '99999';
                    logDivStyle.position = 'absolute';
                    logDivStyle.top = logDivStyle.left = '0';
                    logList = document.createElement('textarea');
                    logList.setAttribute('rows', '20');
                    logList.setAttribute('cols', '30');
                    logList.setAttribute('disabled', 'true');
                    const logListStyle = logList.style;
                    logListStyle.backgroundColor = 'transparent';
                    logListStyle.borderBottom = '1px solid #cccccc';
                    logListStyle.borderTopWidth = logListStyle.borderLeftWidth = logListStyle.borderRightWidth = '0px';
                    logListStyle.borderTopStyle = logListStyle.borderLeftStyle = logListStyle.borderRightStyle = 'none';
                    logListStyle.padding = '0px';
                    logListStyle.margin = '0px';
                    logDiv.appendChild(logList);
                    global_exports_1.legacyCC.game.canvas.parentNode.appendChild(logDiv);
                }
                logList.value = `${logList.value + msg}\r\n`;
                logList.scrollTop = logList.scrollHeight;
            };
            ccError = (message, ...optionalParams) => {
                logToWebPage(`ERROR :  ${formatString(message, ...optionalParams)}`);
            };
            ccAssert = (condition, message, ...optionalParams) => {
                if (!condition) {
                    logToWebPage(`ASSERT: ${formatString(message, ...optionalParams)}`);
                }
            };
            if (mode !== DebugMode.ERROR_FOR_WEB_PAGE) {
                ccWarn = (message, ...optionalParams) => {
                    logToWebPage(`WARN :  ${formatString(message, ...optionalParams)}`);
                };
            }
            if (mode === DebugMode.INFO_FOR_WEB_PAGE) {
                ccLog = (message, ...optionalParams) => {
                    logToWebPage(formatString(message, ...optionalParams));
                };
            }
        }
        else if (console) {
            // Log to console.
            // For JSB
            if (!console.error) {
                console.error = console.log;
            }
            if (!console.warn) {
                console.warn = console.log;
            }
            if (internal_constants_2.EDITOR || console.error.bind) {
                // use bind to avoid pollute call stacks
                ccError = console.error.bind(console);
            }
            else {
                ccError = internal_constants_2.JSB ? console.error : (message, ...optionalParams) => console.error.apply(console, [message, ...optionalParams]);
            }
            ccAssert = (condition, message, ...optionalParams) => {
                if (!condition) {
                    const errorText = formatString(message, ...optionalParams);
                    if (internal_constants_2.DEV) {
                        // eslint-disable-next-line no-debugger
                        debugger;
                    }
                    else {
                        throw new Error(errorText);
                    }
                }
            };
        }
        if (mode !== DebugMode.ERROR) {
            if (internal_constants_2.EDITOR) {
                ccWarn = console.warn.bind(console);
            }
            else if (console.warn.bind) {
                // use bind to avoid pollute call stacks
                ccWarn = console.warn.bind(console);
            }
            else {
                ccWarn = internal_constants_2.JSB ? console.warn : (message, ...optionalParams) => console.warn.apply(console, [message, ...optionalParams]);
            }
        }
        if (internal_constants_2.EDITOR) {
            ccLog = console.log.bind(console);
        }
        else if (mode === DebugMode.INFO) {
            if (internal_constants_2.JSB) {
                // @ts-expect-error We have no typing for this
                if (scriptEngineType === 'JavaScriptCore') {
                    // console.log has to use `console` as its context for iOS 8~9. Therefore, apply it.
                    ccLog = (message, ...optionalParams) => console.log.apply(console, [message, ...optionalParams]);
                }
                else {
                    ccLog = console.log;
                }
            }
            else if (console.log.bind) {
                // use bind to avoid pollute call stacks
                ccLog = console.log.bind(console);
            }
            else {
                ccLog = (message, ...optionalParams) => console.log.apply(console, [message, ...optionalParams]);
            }
        }
        if (mode <= DebugMode.VERBOSE) {
            if (typeof console.debug === 'function') {
                const vendorDebug = console.debug.bind(console);
                ccDebug = (...data) => vendorDebug(...data);
            }
        }
    }
    exports_22("_resetDebugSetting", _resetDebugSetting);
    function _throw(error_) {
        if (internal_constants_2.EDITOR) {
            return error(error_);
        }
        else {
            const stack = error_.stack;
            if (stack) {
                error(internal_constants_2.JSB ? (`${error_}\n${stack}`) : stack);
            }
            else {
                error(error_);
            }
            return undefined;
        }
    }
    exports_22("_throw", _throw);
    function getTypedFormatter(type) {
        return (id, ...args) => {
            const msg = internal_constants_2.DEBUG ? (DebugInfos_1.default[id] || 'unknown id') : `${type} ${id}, please go to ${ERROR_MAP_URL}#${id} to see details.`;
            if (args.length === 0) {
                return msg;
            }
            // eslint-disable-next-line @typescript-eslint/no-unsafe-return
            return internal_constants_2.DEBUG ? formatString(msg, ...args) : `${msg} Arguments: ${args.join(', ')}`;
        };
    }
    function logID(id, ...optionalParams) {
        log(logFormatter(id, ...optionalParams));
    }
    exports_22("logID", logID);
    function warnID(id, ...optionalParams) {
        warn(warnFormatter(id, ...optionalParams));
    }
    exports_22("warnID", warnID);
    function errorID(id, ...optionalParams) {
        error(errorFormatter(id, ...optionalParams));
    }
    exports_22("errorID", errorID);
    function assertID(condition, id, ...optionalParams) {
        if (condition) {
            return;
        }
        assert(false, assertFormatter(id, ...optionalParams));
    }
    exports_22("assertID", assertID);
    /**
     * @en Gets error message with the error id and possible parameters.
     * @zh 通过 error id 和必要的参数来获取错误信息。
     */
    function getError(errorId, ...param) {
        // eslint-disable-next-line @typescript-eslint/no-unsafe-return
        return errorFormatter(errorId, ...param);
    }
    exports_22("getError", getError);
    /**
     * @en Returns whether or not to display the FPS and debug information.
     * @zh 是否显示 FPS 信息和部分调试信息。
     */
    function isDisplayStats() {
        // eslint-disable-next-line @typescript-eslint/no-unsafe-return
        return global_exports_1.legacyCC.profiler ? global_exports_1.legacyCC.profiler.isShowingStats() : false;
    }
    exports_22("isDisplayStats", isDisplayStats);
    /**
     * @en Sets whether display the FPS and debug informations on the bottom-left corner.
     * @zh 设置是否在左下角显示 FPS 和部分调试。
     */
    function setDisplayStats(displayStats) {
        if (global_exports_1.legacyCC.profiler) {
            displayStats ? global_exports_1.legacyCC.profiler.showStats() : global_exports_1.legacyCC.profiler.hideStats();
            global_exports_1.legacyCC.game.config.showFPS = !!displayStats;
        }
    }
    exports_22("setDisplayStats", setDisplayStats);
    return {
        setters: [
            function (internal_constants_2_1) {
                internal_constants_2 = internal_constants_2_1;
            },
            function (DebugInfos_1_1) {
                DebugInfos_1 = DebugInfos_1_1;
            },
            function (global_exports_1_1) {
                global_exports_1 = global_exports_1_1;
            }
        ],
        execute: function () {
            ERROR_MAP_URL = 'https://github.com/cocos-creator/engine/blob/3d/EngineErrorMap.md';
            // The html element displays log in web page (DebugMode.INFO_FOR_WEB_PAGE)
            logList = null;
            ccLog = console.log.bind(console);
            ccWarn = ccLog;
            ccError = ccLog;
            ccAssert = (condition, message, ...optionalParams) => {
                if (!condition) {
                    console.log(`ASSERT: ${formatString(message, ...optionalParams)}`);
                }
            };
            ccDebug = ccLog;
            logFormatter = getTypedFormatter('Log');
            warnFormatter = getTypedFormatter('Warning');
            errorFormatter = getTypedFormatter('Error');
            assertFormatter = getTypedFormatter('Assert');
            /**
             * @en Enum for debug modes.
             * @zh 调试模式。
             */
            (function (DebugMode) {
                /**
                 * @en The debug mode none.
                 * @zh 禁止模式，禁止显示任何日志消息。
                 */
                DebugMode[DebugMode["NONE"] = 0] = "NONE";
                /**
                 * @en The debug mode none.
                 * @zh 调试模式，显示所有日志消息。
                 */
                DebugMode[DebugMode["VERBOSE"] = 1] = "VERBOSE";
                /**
                 * @en Information mode, which display messages with level higher than "information" level.
                 * @zh 信息模式，显示“信息”级别以上的日志消息。
                 */
                DebugMode[DebugMode["INFO"] = 2] = "INFO";
                /**
                 * @en Information mode, which display messages with level higher than "warning" level.
                 * @zh 警告模式，显示“警告”级别以上的日志消息。
                 */
                DebugMode[DebugMode["WARN"] = 3] = "WARN";
                /**
                 * @en Information mode, which display only messages with "error" level.
                 * @zh 错误模式，仅显示“错误”级别的日志消息。
                 */
                DebugMode[DebugMode["ERROR"] = 4] = "ERROR";
                /**
                 * @en The debug mode info for web page.
                 * @zh 信息模式（仅 WEB 端有效），在画面上输出所有信息。
                 */
                DebugMode[DebugMode["INFO_FOR_WEB_PAGE"] = 5] = "INFO_FOR_WEB_PAGE";
                /**
                 * @en The debug mode warn for web page.
                 * @zh 警告模式（仅 WEB 端有效），在画面上输出 warn 级别以上的（包含 error）信息。
                 */
                DebugMode[DebugMode["WARN_FOR_WEB_PAGE"] = 6] = "WARN_FOR_WEB_PAGE";
                /**
                 * @en The debug mode error for web page.
                 * @zh 错误模式（仅 WEB 端有效），在画面上输出 error 信息。
                 */
                DebugMode[DebugMode["ERROR_FOR_WEB_PAGE"] = 7] = "ERROR_FOR_WEB_PAGE";
            })(DebugMode || (DebugMode = {}));
            exports_22("DebugMode", DebugMode);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-command-allocator", ["engine/cocos/core/memop/cached-array", "engine/cocos/core/gfx/webgl/webgl-commands"], function (exports_23, context_23) {
    "use strict";
    var cached_array_1, webgl_commands_1, WebGLCommandPool, WebGLCommandAllocator;
    var __moduleName = context_23 && context_23.id;
    return {
        setters: [
            function (cached_array_1_1) {
                cached_array_1 = cached_array_1_1;
            },
            function (webgl_commands_1_1) {
                webgl_commands_1 = webgl_commands_1_1;
            }
        ],
        execute: function () {
            WebGLCommandPool = class WebGLCommandPool {
                constructor(Clazz, count) {
                    this._freeIdx = 0;
                    this._frees = new Array(count);
                    this._freeCmds = new cached_array_1.CachedArray(count);
                    for (let i = 0; i < count; ++i) {
                        this._frees[i] = new Clazz();
                    }
                    this._freeIdx = count - 1;
                }
                /*
                public alloc (clazz: new() => T): T {
                    return new clazz();
                }
                */
                alloc(Clazz) {
                    if (this._freeIdx < 0) {
                        const size = this._frees.length * 2;
                        const temp = this._frees;
                        this._frees = new Array(size);
                        const increase = size - temp.length;
                        for (let i = 0; i < increase; ++i) {
                            this._frees[i] = new Clazz();
                        }
                        for (let i = increase, j = 0; i < size; ++i, ++j) {
                            this._frees[i] = temp[j];
                        }
                        this._freeIdx += increase;
                    }
                    const cmd = this._frees[this._freeIdx];
                    this._frees[this._freeIdx--] = null;
                    ++cmd.refCount;
                    return cmd;
                }
                free(cmd) {
                    if (--cmd.refCount === 0) {
                        this._freeCmds.push(cmd);
                    }
                }
                freeCmds(cmds) {
                    // return ;
                    for (let i = 0; i < cmds.length; ++i) {
                        if (--cmds.array[i].refCount === 0) {
                            this._freeCmds.push(cmds.array[i]);
                        }
                    }
                }
                release() {
                    for (let i = 0; i < this._freeCmds.length; ++i) {
                        const cmd = this._freeCmds.array[i];
                        cmd.clear();
                        this._frees[++this._freeIdx] = cmd;
                    }
                    this._freeCmds.clear();
                }
            };
            exports_23("WebGLCommandPool", WebGLCommandPool);
            WebGLCommandAllocator = class WebGLCommandAllocator {
                constructor() {
                    this.beginRenderPassCmdPool = new WebGLCommandPool(webgl_commands_1.WebGLCmdBeginRenderPass, 1);
                    this.bindStatesCmdPool = new WebGLCommandPool(webgl_commands_1.WebGLCmdBindStates, 1);
                    this.drawCmdPool = new WebGLCommandPool(webgl_commands_1.WebGLCmdDraw, 1);
                    this.updateBufferCmdPool = new WebGLCommandPool(webgl_commands_1.WebGLCmdUpdateBuffer, 1);
                    this.copyBufferToTextureCmdPool = new WebGLCommandPool(webgl_commands_1.WebGLCmdCopyBufferToTexture, 1);
                }
                clearCmds(cmdPackage) {
                    if (cmdPackage.beginRenderPassCmds.length) {
                        this.beginRenderPassCmdPool.freeCmds(cmdPackage.beginRenderPassCmds);
                        cmdPackage.beginRenderPassCmds.clear();
                    }
                    if (cmdPackage.bindStatesCmds.length) {
                        this.bindStatesCmdPool.freeCmds(cmdPackage.bindStatesCmds);
                        cmdPackage.bindStatesCmds.clear();
                    }
                    if (cmdPackage.drawCmds.length) {
                        this.drawCmdPool.freeCmds(cmdPackage.drawCmds);
                        cmdPackage.drawCmds.clear();
                    }
                    if (cmdPackage.updateBufferCmds.length) {
                        this.updateBufferCmdPool.freeCmds(cmdPackage.updateBufferCmds);
                        cmdPackage.updateBufferCmds.clear();
                    }
                    if (cmdPackage.copyBufferToTextureCmds.length) {
                        this.copyBufferToTextureCmdPool.freeCmds(cmdPackage.copyBufferToTextureCmds);
                        cmdPackage.copyBufferToTextureCmds.clear();
                    }
                    cmdPackage.cmds.clear();
                }
                releaseCmds() {
                    this.beginRenderPassCmdPool.release();
                    this.bindStatesCmdPool.release();
                    this.drawCmdPool.release();
                    this.updateBufferCmdPool.release();
                    this.copyBufferToTextureCmdPool.release();
                }
            };
            exports_23("WebGLCommandAllocator", WebGLCommandAllocator);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-define", [], function (exports_24, context_24) {
    "use strict";
    var WebGLEXT, WebGLDeviceManager;
    var __moduleName = context_24 && context_24.id;
    return {
        setters: [],
        execute: function () {
            // Extensions
            (function (WebGLEXT) {
                WebGLEXT[WebGLEXT["RGBA16F_EXT"] = 34842] = "RGBA16F_EXT";
                WebGLEXT[WebGLEXT["RGB16F_EXT"] = 34843] = "RGB16F_EXT";
                WebGLEXT[WebGLEXT["RGBA32F_EXT"] = 34836] = "RGBA32F_EXT";
                WebGLEXT[WebGLEXT["FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT"] = 33297] = "FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT";
                WebGLEXT[WebGLEXT["UNSIGNED_NORMALIZED_EXT"] = 35863] = "UNSIGNED_NORMALIZED_EXT";
                WebGLEXT[WebGLEXT["UNSIGNED_INT_24_8_WEBGL"] = 34042] = "UNSIGNED_INT_24_8_WEBGL";
                WebGLEXT[WebGLEXT["HALF_FLOAT_OES"] = 36193] = "HALF_FLOAT_OES";
                WebGLEXT[WebGLEXT["SRGB_EXT"] = 35904] = "SRGB_EXT";
                WebGLEXT[WebGLEXT["SRGB_ALPHA_EXT"] = 35906] = "SRGB_ALPHA_EXT";
                WebGLEXT[WebGLEXT["SRGB8_ALPHA8_EXT"] = 35907] = "SRGB8_ALPHA8_EXT";
                WebGLEXT[WebGLEXT["COMPRESSED_RGB_S3TC_DXT1_EXT"] = 33776] = "COMPRESSED_RGB_S3TC_DXT1_EXT";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_S3TC_DXT1_EXT"] = 33777] = "COMPRESSED_RGBA_S3TC_DXT1_EXT";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_S3TC_DXT3_EXT"] = 33778] = "COMPRESSED_RGBA_S3TC_DXT3_EXT";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_S3TC_DXT5_EXT"] = 33779] = "COMPRESSED_RGBA_S3TC_DXT5_EXT";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB_S3TC_DXT1_EXT"] = 35916] = "COMPRESSED_SRGB_S3TC_DXT1_EXT";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT"] = 35917] = "COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT"] = 35918] = "COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT"] = 35919] = "COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
                WebGLEXT[WebGLEXT["COMPRESSED_RGB_PVRTC_4BPPV1_IMG"] = 35840] = "COMPRESSED_RGB_PVRTC_4BPPV1_IMG";
                WebGLEXT[WebGLEXT["COMPRESSED_RGB_PVRTC_2BPPV1_IMG"] = 35841] = "COMPRESSED_RGB_PVRTC_2BPPV1_IMG";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_PVRTC_4BPPV1_IMG"] = 35842] = "COMPRESSED_RGBA_PVRTC_4BPPV1_IMG";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_PVRTC_2BPPV1_IMG"] = 35843] = "COMPRESSED_RGBA_PVRTC_2BPPV1_IMG";
                WebGLEXT[WebGLEXT["COMPRESSED_RGB_ETC1_WEBGL"] = 36196] = "COMPRESSED_RGB_ETC1_WEBGL";
                WebGLEXT[WebGLEXT["COMPRESSED_R11_EAC"] = 37488] = "COMPRESSED_R11_EAC";
                WebGLEXT[WebGLEXT["COMPRESSED_SIGNED_R11_EAC"] = 37489] = "COMPRESSED_SIGNED_R11_EAC";
                WebGLEXT[WebGLEXT["COMPRESSED_RG11_EAC"] = 37490] = "COMPRESSED_RG11_EAC";
                WebGLEXT[WebGLEXT["COMPRESSED_SIGNED_RG11_EAC"] = 37491] = "COMPRESSED_SIGNED_RG11_EAC";
                WebGLEXT[WebGLEXT["COMPRESSED_RGB8_ETC2"] = 37492] = "COMPRESSED_RGB8_ETC2";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ETC2"] = 37493] = "COMPRESSED_SRGB8_ETC2";
                WebGLEXT[WebGLEXT["COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2"] = 37494] = "COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2"] = 37495] = "COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA8_ETC2_EAC"] = 37496] = "COMPRESSED_RGBA8_ETC2_EAC";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ETC2_EAC"] = 37497] = "COMPRESSED_SRGB8_ALPHA8_ETC2_EAC";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_4x4_KHR"] = 37808] = "COMPRESSED_RGBA_ASTC_4x4_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_5x4_KHR"] = 37809] = "COMPRESSED_RGBA_ASTC_5x4_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_5x5_KHR"] = 37810] = "COMPRESSED_RGBA_ASTC_5x5_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_6x5_KHR"] = 37811] = "COMPRESSED_RGBA_ASTC_6x5_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_6x6_KHR"] = 37812] = "COMPRESSED_RGBA_ASTC_6x6_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_8x5_KHR"] = 37813] = "COMPRESSED_RGBA_ASTC_8x5_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_8x6_KHR"] = 37814] = "COMPRESSED_RGBA_ASTC_8x6_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_8x8_KHR"] = 37815] = "COMPRESSED_RGBA_ASTC_8x8_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_10x5_KHR"] = 37816] = "COMPRESSED_RGBA_ASTC_10x5_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_10x6_KHR"] = 37817] = "COMPRESSED_RGBA_ASTC_10x6_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_10x8_KHR"] = 37818] = "COMPRESSED_RGBA_ASTC_10x8_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_10x10_KHR"] = 37819] = "COMPRESSED_RGBA_ASTC_10x10_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_12x10_KHR"] = 37820] = "COMPRESSED_RGBA_ASTC_12x10_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_RGBA_ASTC_12x12_KHR"] = 37821] = "COMPRESSED_RGBA_ASTC_12x12_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR"] = 37840] = "COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR"] = 37841] = "COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR"] = 37842] = "COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR"] = 37843] = "COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR"] = 37844] = "COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR"] = 37845] = "COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR"] = 37846] = "COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR"] = 37847] = "COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR"] = 37848] = "COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR"] = 37849] = "COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR"] = 37850] = "COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR"] = 37851] = "COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR"] = 37852] = "COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR";
                WebGLEXT[WebGLEXT["COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR"] = 37853] = "COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR";
            })(WebGLEXT || (WebGLEXT = {}));
            exports_24("WebGLEXT", WebGLEXT);
            // put the global instance here so that we won't have circular dependencies
            WebGLDeviceManager = class WebGLDeviceManager {
                static get instance() {
                    return WebGLDeviceManager._instance;
                }
                static setInstance(instance) {
                    WebGLDeviceManager._instance = instance;
                }
            };
            exports_24("WebGLDeviceManager", WebGLDeviceManager);
            WebGLDeviceManager._instance = null;
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/math/bits", [], function (exports_25, context_25) {
    "use strict";
    var INT_BITS, INT_MAX, INT_MIN, REVERSE_TABLE;
    var __moduleName = context_25 && context_25.id;
    /**
     * @en Returns -1, 0, +1 depending on sign of x.
     * @zh 根据x的符号返回 -1，0，+1。
     */
    function sign(v) {
        return (v > 0) - (v < 0);
    }
    exports_25("sign", sign);
    /**
     * @en Computes absolute value of integer.
     * @zh 计算整数的绝对值。
     */
    function abs(v) {
        const mask = v >> (INT_BITS - 1);
        return (v ^ mask) - mask;
    }
    exports_25("abs", abs);
    /**
     * @en Computes minimum of integers x and y.
     * @zh 计算整数x和y中的最小值。
     */
    function min(x, y) {
        return y ^ ((x ^ y) & -(x < y));
    }
    exports_25("min", min);
    /**
     * @en Computes maximum of integers x and y.
     * @zh 计算整数x和y中的最大值。
     */
    function max(x, y) {
        return x ^ ((x ^ y) & -(x < y));
    }
    exports_25("max", max);
    /**
     * @en Checks if a number is a power of two.
     * @zh 检查一个数字是否是2的幂。
     */
    function isPow2(v) {
        return !(v & (v - 1)) && (!!v);
    }
    exports_25("isPow2", isPow2);
    /**
     * @en Computes log base 2 of v.
     * @zh 计算以 2 为底的 v 的对数。
     */
    function log2(v) {
        let r;
        let shift;
        r = (v > 0xFFFF) << 4;
        v >>>= r;
        shift = (v > 0xFF) << 3;
        v >>>= shift;
        r |= shift;
        shift = (v > 0xF) << 2;
        v >>>= shift;
        r |= shift;
        shift = (v > 0x3) << 1;
        v >>>= shift;
        r |= shift;
        return r | (v >> 1);
    }
    exports_25("log2", log2);
    /**
     * @en Computes log base 10 of v.
     * @zh 计算以 10 为底的 v 的对数。
     */
    function log10(v) {
        return (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7
            : (v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4
                : (v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;
    }
    exports_25("log10", log10);
    /**
     * @zh Counts number of bits.
     * @en 计算传入数字二进制表示中 1 的数量。
     */
    function popCount(v) {
        v -= ((v >>> 1) & 0x55555555);
        v = (v & 0x33333333) + ((v >>> 2) & 0x33333333);
        return ((v + (v >>> 4) & 0xF0F0F0F) * 0x1010101) >>> 24;
    }
    exports_25("popCount", popCount);
    /**
     * @en Counts number of trailing zeros.
     * @zh 计算传入数字二进制表示尾随零的数量。
     */
    function countTrailingZeros(v) {
        let c = 32;
        v &= -v;
        if (v) {
            c--;
        }
        if (v & 0x0000FFFF) {
            c -= 16;
        }
        if (v & 0x00FF00FF) {
            c -= 8;
        }
        if (v & 0x0F0F0F0F) {
            c -= 4;
        }
        if (v & 0x33333333) {
            c -= 2;
        }
        if (v & 0x55555555) {
            c -= 1;
        }
        return c;
    }
    exports_25("countTrailingZeros", countTrailingZeros);
    /**
     * @en Rounds to next power of 2.
     * @zh 计算大于等于v的最小的二的整数次幂的数字。
     */
    function nextPow2(v) {
        --v;
        v |= v >>> 1;
        v |= v >>> 2;
        v |= v >>> 4;
        v |= v >>> 8;
        v |= v >>> 16;
        return v + 1;
    }
    exports_25("nextPow2", nextPow2);
    /**
     * @en Rounds down to previous power of 2.
     * @zh 计算小于等于v的最小的二的整数次幂的数字。
     */
    function prevPow2(v) {
        v |= v >>> 1;
        v |= v >>> 2;
        v |= v >>> 4;
        v |= v >>> 8;
        v |= v >>> 16;
        return v - (v >>> 1);
    }
    exports_25("prevPow2", prevPow2);
    /**
     * @en Computes parity of word.
     * @zh 奇偶校验。
     */
    function parity(v) {
        v ^= v >>> 16;
        v ^= v >>> 8;
        v ^= v >>> 4;
        v &= 0xf;
        return (0x6996 >>> v) & 1;
    }
    exports_25("parity", parity);
    /**
     * @en Reverse bits in a 32 bit word.
     * @zh 翻转 32 位二进制数字。
     */
    function reverse(v) {
        return (REVERSE_TABLE[v & 0xff] << 24)
            | (REVERSE_TABLE[(v >>> 8) & 0xff] << 16)
            | (REVERSE_TABLE[(v >>> 16) & 0xff] << 8)
            | REVERSE_TABLE[(v >>> 24) & 0xff];
    }
    exports_25("reverse", reverse);
    /**
     * @en Interleave bits of 2 coordinates with 16 bits. Useful for fast quadtree codes.
     * @zh 将两个 16 位数字按位交错编码。有利于在快速四叉树中使用。
     */
    function interleave2(x, y) {
        x &= 0xFFFF;
        x = (x | (x << 8)) & 0x00FF00FF;
        x = (x | (x << 4)) & 0x0F0F0F0F;
        x = (x | (x << 2)) & 0x33333333;
        x = (x | (x << 1)) & 0x55555555;
        y &= 0xFFFF;
        y = (y | (y << 8)) & 0x00FF00FF;
        y = (y | (y << 4)) & 0x0F0F0F0F;
        y = (y | (y << 2)) & 0x33333333;
        y = (y | (y << 1)) & 0x55555555;
        return x | (y << 1);
    }
    exports_25("interleave2", interleave2);
    /**
     * @en Extracts the nth interleaved component.
     * @zh 提取第 n 个交错分量。
     */
    function deinterleave2(v, n) {
        v = (v >>> n) & 0x55555555;
        v = (v | (v >>> 1)) & 0x33333333;
        v = (v | (v >>> 2)) & 0x0F0F0F0F;
        v = (v | (v >>> 4)) & 0x00FF00FF;
        v = (v | (v >>> 16)) & 0x000FFFF;
        return (v << 16) >> 16;
    }
    exports_25("deinterleave2", deinterleave2);
    /**
     * @en Interleave bits of 3 coordinates, each with 10 bits.  Useful for fast octree codes.
     * @zh 将三个数字按位交错编码，每个数字占十位。有利于在八叉树中使用。
     */
    function interleave3(x, y, z) {
        x &= 0x3FF;
        x = (x | (x << 16)) & 4278190335;
        x = (x | (x << 8)) & 251719695;
        x = (x | (x << 4)) & 3272356035;
        x = (x | (x << 2)) & 1227133513;
        y &= 0x3FF;
        y = (y | (y << 16)) & 4278190335;
        y = (y | (y << 8)) & 251719695;
        y = (y | (y << 4)) & 3272356035;
        y = (y | (y << 2)) & 1227133513;
        x |= (y << 1);
        z &= 0x3FF;
        z = (z | (z << 16)) & 4278190335;
        z = (z | (z << 8)) & 251719695;
        z = (z | (z << 4)) & 3272356035;
        z = (z | (z << 2)) & 1227133513;
        return x | (z << 2);
    }
    exports_25("interleave3", interleave3);
    /**
     * @zh Extracts nth interleaved component of a 3-tuple.
     * @en 提取三个数字中的第n个交错分量。
     */
    function deinterleave3(v, n) {
        v = (v >>> n) & 1227133513;
        v = (v | (v >>> 2)) & 3272356035;
        v = (v | (v >>> 4)) & 251719695;
        v = (v | (v >>> 8)) & 4278190335;
        v = (v | (v >>> 16)) & 0x3FF;
        return (v << 22) >> 22;
    }
    exports_25("deinterleave3", deinterleave3);
    /**
     * @en Compute the lexicographically next bit permutation
     * @zh 计算下一组字典序的比特排列
     */
    function nextCombination(v) {
        const t = v | (v - 1);
        return (t + 1) | (((~t & -~t) - 1) >>> (countTrailingZeros(v) + 1));
    }
    exports_25("nextCombination", nextCombination);
    return {
        setters: [],
        execute: function () {
            /**
             * @packageDocumentation
             * 数学库
             * @module core/math/bits
             */
            /**
             * Bit twiddling hacks for JavaScript.
             *
             * Author: Mikola Lysenko
             *
             * Ported from Stanford bit twiddling hack library:
             *    http://graphics.stanford.edu/~seander/bithacks.html
             */
            // Number of bits in an integer
            exports_25("INT_BITS", INT_BITS = 32);
            exports_25("INT_MAX", INT_MAX = 0x7fffffff);
            exports_25("INT_MIN", INT_MIN = -1 << (INT_BITS - 1));
            REVERSE_TABLE = new Array(256);
            ((tab) => {
                for (let i = 0; i < 256; ++i) {
                    let v = i;
                    let r = i;
                    let s = 7;
                    for (v >>>= 1; v; v >>>= 1) {
                        r <<= 1;
                        r |= v & 1;
                        --s;
                    }
                    tab[i] = (r << s) & 0xff;
                }
            })(REVERSE_TABLE);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-gpu-objects", ["engine/cocos/core/math/bits"], function (exports_26, context_26) {
    "use strict";
    var bits_1, WebGLIndirectDrawInfos;
    var __moduleName = context_26 && context_26.id;
    return {
        setters: [
            function (bits_1_1) {
                bits_1 = bits_1_1;
            }
        ],
        execute: function () {
            WebGLIndirectDrawInfos = class WebGLIndirectDrawInfos {
                constructor() {
                    this.drawCount = 0;
                    this.drawByIndex = false;
                    this.instancedDraw = false;
                    this._capacity = 4;
                    this.counts = new Int32Array(this._capacity);
                    this.offsets = new Int32Array(this._capacity);
                    this.instances = new Int32Array(this._capacity);
                    this.byteOffsets = new Int32Array(this._capacity);
                }
                clearDraws() {
                    this.drawCount = 0;
                    this.drawByIndex = false;
                    this.instancedDraw = false;
                }
                setDrawInfo(idx, info) {
                    this._ensureCapacity(idx);
                    this.drawByIndex = info.indexCount > 0;
                    this.instancedDraw = info.instanceCount > 1;
                    this.drawCount = Math.max(idx + 1, this.drawCount);
                    if (this.drawByIndex) {
                        this.counts[idx] = info.indexCount;
                        this.offsets[idx] = info.firstIndex;
                    }
                    else {
                        this.counts[idx] = info.vertexCount;
                        this.offsets[idx] = info.firstVertex;
                    }
                    this.instances[idx] = Math.max(1, info.instanceCount);
                }
                _ensureCapacity(target) {
                    if (this._capacity > target)
                        return;
                    this._capacity = bits_1.nextPow2(target);
                    const counts = new Int32Array(this._capacity);
                    const offsets = new Int32Array(this._capacity);
                    const instances = new Int32Array(this._capacity);
                    this.byteOffsets = new Int32Array(this._capacity);
                    counts.set(this.counts);
                    offsets.set(this.offsets);
                    instances.set(this.instances);
                    this.counts = counts;
                    this.offsets = offsets;
                    this.instances = instances;
                }
            };
            exports_26("WebGLIndirectDrawInfos", WebGLIndirectDrawInfos);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-commands", ["engine/cocos/core/memop/cached-array", "engine/cocos/core/platform/debug", "engine/cocos/core/gfx/webgl/webgl-define", "engine/cocos/core/gfx/base/define"], function (exports_27, context_27) {
    "use strict";
    var cached_array_2, debug_1, webgl_define_1, define_18, WebGLCmpFuncs, WebGLStencilOps, WebGLBlendOps, WebGLBlendFactors, WebGLCmd, WebGLCmdObject, WebGLCmdBeginRenderPass, WebGLCmdBindStates, WebGLCmdDraw, WebGLCmdUpdateBuffer, WebGLCmdCopyBufferToTexture, WebGLCmdPackage, gfxStateCache, cmdIds;
    var __moduleName = context_27 && context_27.id;
    function GFXFormatToWebGLType(format, gl) {
        switch (format) {
            case define_18.Format.R8: return gl.UNSIGNED_BYTE;
            case define_18.Format.R8SN: return gl.BYTE;
            case define_18.Format.R8UI: return gl.UNSIGNED_BYTE;
            case define_18.Format.R8I: return gl.BYTE;
            case define_18.Format.R16F: return webgl_define_1.WebGLEXT.HALF_FLOAT_OES;
            case define_18.Format.R16UI: return gl.UNSIGNED_SHORT;
            case define_18.Format.R16I: return gl.SHORT;
            case define_18.Format.R32F: return gl.FLOAT;
            case define_18.Format.R32UI: return gl.UNSIGNED_INT;
            case define_18.Format.R32I: return gl.INT;
            case define_18.Format.RG8: return gl.UNSIGNED_BYTE;
            case define_18.Format.RG8SN: return gl.BYTE;
            case define_18.Format.RG8UI: return gl.UNSIGNED_BYTE;
            case define_18.Format.RG8I: return gl.BYTE;
            case define_18.Format.RG16F: return webgl_define_1.WebGLEXT.HALF_FLOAT_OES;
            case define_18.Format.RG16UI: return gl.UNSIGNED_SHORT;
            case define_18.Format.RG16I: return gl.SHORT;
            case define_18.Format.RG32F: return gl.FLOAT;
            case define_18.Format.RG32UI: return gl.UNSIGNED_INT;
            case define_18.Format.RG32I: return gl.INT;
            case define_18.Format.RGB8: return gl.UNSIGNED_BYTE;
            case define_18.Format.SRGB8: return gl.UNSIGNED_BYTE;
            case define_18.Format.RGB8SN: return gl.BYTE;
            case define_18.Format.RGB8UI: return gl.UNSIGNED_BYTE;
            case define_18.Format.RGB8I: return gl.BYTE;
            case define_18.Format.RGB16F: return webgl_define_1.WebGLEXT.HALF_FLOAT_OES;
            case define_18.Format.RGB16UI: return gl.UNSIGNED_SHORT;
            case define_18.Format.RGB16I: return gl.SHORT;
            case define_18.Format.RGB32F: return gl.FLOAT;
            case define_18.Format.RGB32UI: return gl.UNSIGNED_INT;
            case define_18.Format.RGB32I: return gl.INT;
            case define_18.Format.BGRA8: return gl.UNSIGNED_BYTE;
            case define_18.Format.RGBA8: return gl.UNSIGNED_BYTE;
            case define_18.Format.SRGB8_A8: return gl.UNSIGNED_BYTE;
            case define_18.Format.RGBA8SN: return gl.BYTE;
            case define_18.Format.RGBA8UI: return gl.UNSIGNED_BYTE;
            case define_18.Format.RGBA8I: return gl.BYTE;
            case define_18.Format.RGBA16F: return webgl_define_1.WebGLEXT.HALF_FLOAT_OES;
            case define_18.Format.RGBA16UI: return gl.UNSIGNED_SHORT;
            case define_18.Format.RGBA16I: return gl.SHORT;
            case define_18.Format.RGBA32F: return gl.FLOAT;
            case define_18.Format.RGBA32UI: return gl.UNSIGNED_INT;
            case define_18.Format.RGBA32I: return gl.INT;
            case define_18.Format.R5G6B5: return gl.UNSIGNED_SHORT_5_6_5;
            case define_18.Format.R11G11B10F: return gl.FLOAT;
            case define_18.Format.RGB5A1: return gl.UNSIGNED_SHORT_5_5_5_1;
            case define_18.Format.RGBA4: return gl.UNSIGNED_SHORT_4_4_4_4;
            case define_18.Format.RGB10A2: return gl.UNSIGNED_BYTE;
            case define_18.Format.RGB10A2UI: return gl.UNSIGNED_INT;
            case define_18.Format.RGB9E5: return gl.UNSIGNED_BYTE;
            case define_18.Format.DEPTH: return gl.UNSIGNED_INT;
            case define_18.Format.DEPTH_STENCIL: return webgl_define_1.WebGLEXT.UNSIGNED_INT_24_8_WEBGL; // not supported, fallback
            case define_18.Format.BC1: return gl.UNSIGNED_BYTE;
            case define_18.Format.BC1_SRGB: return gl.UNSIGNED_BYTE;
            case define_18.Format.BC2: return gl.UNSIGNED_BYTE;
            case define_18.Format.BC2_SRGB: return gl.UNSIGNED_BYTE;
            case define_18.Format.BC3: return gl.UNSIGNED_BYTE;
            case define_18.Format.BC3_SRGB: return gl.UNSIGNED_BYTE;
            case define_18.Format.BC4: return gl.UNSIGNED_BYTE;
            case define_18.Format.BC4_SNORM: return gl.BYTE;
            case define_18.Format.BC5: return gl.UNSIGNED_BYTE;
            case define_18.Format.BC5_SNORM: return gl.BYTE;
            case define_18.Format.BC6H_SF16: return gl.FLOAT;
            case define_18.Format.BC6H_UF16: return gl.FLOAT;
            case define_18.Format.BC7: return gl.UNSIGNED_BYTE;
            case define_18.Format.BC7_SRGB: return gl.UNSIGNED_BYTE;
            case define_18.Format.ETC_RGB8: return gl.UNSIGNED_BYTE;
            case define_18.Format.ETC2_RGB8: return gl.UNSIGNED_BYTE;
            case define_18.Format.ETC2_SRGB8: return gl.UNSIGNED_BYTE;
            case define_18.Format.ETC2_RGB8_A1: return gl.UNSIGNED_BYTE;
            case define_18.Format.ETC2_SRGB8_A1: return gl.UNSIGNED_BYTE;
            case define_18.Format.EAC_R11: return gl.UNSIGNED_BYTE;
            case define_18.Format.EAC_R11SN: return gl.BYTE;
            case define_18.Format.EAC_RG11: return gl.UNSIGNED_BYTE;
            case define_18.Format.EAC_RG11SN: return gl.BYTE;
            case define_18.Format.PVRTC_RGB2: return gl.UNSIGNED_BYTE;
            case define_18.Format.PVRTC_RGBA2: return gl.UNSIGNED_BYTE;
            case define_18.Format.PVRTC_RGB4: return gl.UNSIGNED_BYTE;
            case define_18.Format.PVRTC_RGBA4: return gl.UNSIGNED_BYTE;
            case define_18.Format.PVRTC2_2BPP: return gl.UNSIGNED_BYTE;
            case define_18.Format.PVRTC2_4BPP: return gl.UNSIGNED_BYTE;
            case define_18.Format.ASTC_RGBA_4X4:
            case define_18.Format.ASTC_RGBA_5X4:
            case define_18.Format.ASTC_RGBA_5X5:
            case define_18.Format.ASTC_RGBA_6X5:
            case define_18.Format.ASTC_RGBA_6X6:
            case define_18.Format.ASTC_RGBA_8X5:
            case define_18.Format.ASTC_RGBA_8X6:
            case define_18.Format.ASTC_RGBA_8X8:
            case define_18.Format.ASTC_RGBA_10X5:
            case define_18.Format.ASTC_RGBA_10X6:
            case define_18.Format.ASTC_RGBA_10X8:
            case define_18.Format.ASTC_RGBA_10X10:
            case define_18.Format.ASTC_RGBA_12X10:
            case define_18.Format.ASTC_RGBA_12X12:
            case define_18.Format.ASTC_SRGBA_4X4:
            case define_18.Format.ASTC_SRGBA_5X4:
            case define_18.Format.ASTC_SRGBA_5X5:
            case define_18.Format.ASTC_SRGBA_6X5:
            case define_18.Format.ASTC_SRGBA_6X6:
            case define_18.Format.ASTC_SRGBA_8X5:
            case define_18.Format.ASTC_SRGBA_8X6:
            case define_18.Format.ASTC_SRGBA_8X8:
            case define_18.Format.ASTC_SRGBA_10X5:
            case define_18.Format.ASTC_SRGBA_10X6:
            case define_18.Format.ASTC_SRGBA_10X8:
            case define_18.Format.ASTC_SRGBA_10X10:
            case define_18.Format.ASTC_SRGBA_12X10:
            case define_18.Format.ASTC_SRGBA_12X12:
                return gl.UNSIGNED_BYTE;
            default: {
                return gl.UNSIGNED_BYTE;
            }
        }
    }
    exports_27("GFXFormatToWebGLType", GFXFormatToWebGLType);
    function GFXFormatToWebGLInternalFormat(format, gl) {
        switch (format) {
            case define_18.Format.R5G6B5: return gl.RGB565;
            case define_18.Format.RGB5A1: return gl.RGB5_A1;
            case define_18.Format.RGBA4: return gl.RGBA4;
            case define_18.Format.RGBA16F: return webgl_define_1.WebGLEXT.RGBA16F_EXT;
            case define_18.Format.RGBA32F: return webgl_define_1.WebGLEXT.RGBA32F_EXT;
            case define_18.Format.SRGB8_A8: return webgl_define_1.WebGLEXT.SRGB8_ALPHA8_EXT;
            case define_18.Format.DEPTH: return gl.DEPTH_COMPONENT16;
            case define_18.Format.DEPTH_STENCIL: return gl.DEPTH_STENCIL;
            default: {
                console.error('Unsupported Format, convert to WebGL internal format failed.');
                return gl.RGBA;
            }
        }
    }
    exports_27("GFXFormatToWebGLInternalFormat", GFXFormatToWebGLInternalFormat);
    function GFXFormatToWebGLFormat(format, gl) {
        switch (format) {
            case define_18.Format.A8: return gl.ALPHA;
            case define_18.Format.L8: return gl.LUMINANCE;
            case define_18.Format.LA8: return gl.LUMINANCE_ALPHA;
            case define_18.Format.RGB8: return gl.RGB;
            case define_18.Format.RGB16F: return gl.RGB;
            case define_18.Format.RGB32F: return gl.RGB;
            case define_18.Format.BGRA8: return gl.RGBA;
            case define_18.Format.RGBA8: return gl.RGBA;
            case define_18.Format.SRGB8_A8: return gl.RGBA;
            case define_18.Format.RGBA16F: return gl.RGBA;
            case define_18.Format.RGBA32F: return gl.RGBA;
            case define_18.Format.R5G6B5: return gl.RGB;
            case define_18.Format.RGB5A1: return gl.RGBA;
            case define_18.Format.RGBA4: return gl.RGBA;
            case define_18.Format.DEPTH: return gl.DEPTH_COMPONENT;
            case define_18.Format.DEPTH_STENCIL: return gl.DEPTH_STENCIL;
            case define_18.Format.BC1: return webgl_define_1.WebGLEXT.COMPRESSED_RGB_S3TC_DXT1_EXT;
            case define_18.Format.BC1_ALPHA: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case define_18.Format.BC1_SRGB: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB_S3TC_DXT1_EXT;
            case define_18.Format.BC1_SRGB_ALPHA: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
            case define_18.Format.BC2: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case define_18.Format.BC2_SRGB: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
            case define_18.Format.BC3: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_S3TC_DXT5_EXT;
            case define_18.Format.BC3_SRGB: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
            case define_18.Format.ETC_RGB8: return webgl_define_1.WebGLEXT.COMPRESSED_RGB_ETC1_WEBGL;
            case define_18.Format.ETC2_RGB8: return webgl_define_1.WebGLEXT.COMPRESSED_RGB8_ETC2;
            case define_18.Format.ETC2_SRGB8: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ETC2;
            case define_18.Format.ETC2_RGB8_A1: return webgl_define_1.WebGLEXT.COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            case define_18.Format.ETC2_SRGB8_A1: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            case define_18.Format.ETC2_RGBA8: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA8_ETC2_EAC;
            case define_18.Format.ETC2_SRGB8_A8: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
            case define_18.Format.EAC_R11: return webgl_define_1.WebGLEXT.COMPRESSED_R11_EAC;
            case define_18.Format.EAC_R11SN: return webgl_define_1.WebGLEXT.COMPRESSED_SIGNED_R11_EAC;
            case define_18.Format.EAC_RG11: return webgl_define_1.WebGLEXT.COMPRESSED_RG11_EAC;
            case define_18.Format.EAC_RG11SN: return webgl_define_1.WebGLEXT.COMPRESSED_SIGNED_RG11_EAC;
            case define_18.Format.PVRTC_RGB2: return webgl_define_1.WebGLEXT.COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
            case define_18.Format.PVRTC_RGBA2: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
            case define_18.Format.PVRTC_RGB4: return webgl_define_1.WebGLEXT.COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
            case define_18.Format.PVRTC_RGBA4: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
            case define_18.Format.ASTC_RGBA_4X4: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_4x4_KHR;
            case define_18.Format.ASTC_RGBA_5X4: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_5x4_KHR;
            case define_18.Format.ASTC_RGBA_5X5: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_5x5_KHR;
            case define_18.Format.ASTC_RGBA_6X5: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_6x5_KHR;
            case define_18.Format.ASTC_RGBA_6X6: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_6x6_KHR;
            case define_18.Format.ASTC_RGBA_8X5: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_8x5_KHR;
            case define_18.Format.ASTC_RGBA_8X6: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_8x6_KHR;
            case define_18.Format.ASTC_RGBA_8X8: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_8x8_KHR;
            case define_18.Format.ASTC_RGBA_10X5: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_10x5_KHR;
            case define_18.Format.ASTC_RGBA_10X6: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_10x6_KHR;
            case define_18.Format.ASTC_RGBA_10X8: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_10x8_KHR;
            case define_18.Format.ASTC_RGBA_10X10: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_10x10_KHR;
            case define_18.Format.ASTC_RGBA_12X10: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_12x10_KHR;
            case define_18.Format.ASTC_RGBA_12X12: return webgl_define_1.WebGLEXT.COMPRESSED_RGBA_ASTC_12x12_KHR;
            case define_18.Format.ASTC_SRGBA_4X4: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
            case define_18.Format.ASTC_SRGBA_5X4: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
            case define_18.Format.ASTC_SRGBA_5X5: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
            case define_18.Format.ASTC_SRGBA_6X5: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
            case define_18.Format.ASTC_SRGBA_6X6: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
            case define_18.Format.ASTC_SRGBA_8X5: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
            case define_18.Format.ASTC_SRGBA_8X6: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
            case define_18.Format.ASTC_SRGBA_8X8: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
            case define_18.Format.ASTC_SRGBA_10X5: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
            case define_18.Format.ASTC_SRGBA_10X6: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
            case define_18.Format.ASTC_SRGBA_10X8: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
            case define_18.Format.ASTC_SRGBA_10X10: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
            case define_18.Format.ASTC_SRGBA_12X10: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
            case define_18.Format.ASTC_SRGBA_12X12: return webgl_define_1.WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
            default: {
                console.error('Unsupported Format, convert to WebGL format failed.');
                return gl.RGBA;
            }
        }
    }
    exports_27("GFXFormatToWebGLFormat", GFXFormatToWebGLFormat);
    function GFXTypeToWebGLType(type, gl) {
        switch (type) {
            case define_18.Type.BOOL: return gl.BOOL;
            case define_18.Type.BOOL2: return gl.BOOL_VEC2;
            case define_18.Type.BOOL3: return gl.BOOL_VEC3;
            case define_18.Type.BOOL4: return gl.BOOL_VEC4;
            case define_18.Type.INT: return gl.INT;
            case define_18.Type.INT2: return gl.INT_VEC2;
            case define_18.Type.INT3: return gl.INT_VEC3;
            case define_18.Type.INT4: return gl.INT_VEC4;
            case define_18.Type.UINT: return gl.UNSIGNED_INT;
            case define_18.Type.FLOAT: return gl.FLOAT;
            case define_18.Type.FLOAT2: return gl.FLOAT_VEC2;
            case define_18.Type.FLOAT3: return gl.FLOAT_VEC3;
            case define_18.Type.FLOAT4: return gl.FLOAT_VEC4;
            case define_18.Type.MAT2: return gl.FLOAT_MAT2;
            case define_18.Type.MAT3: return gl.FLOAT_MAT3;
            case define_18.Type.MAT4: return gl.FLOAT_MAT4;
            case define_18.Type.SAMPLER2D: return gl.SAMPLER_2D;
            case define_18.Type.SAMPLER_CUBE: return gl.SAMPLER_CUBE;
            default: {
                console.error('Unsupported GLType, convert to GL type failed.');
                return define_18.Type.UNKNOWN;
            }
        }
    }
    function GFXTypeToTypedArrayCtor(type) {
        switch (type) {
            case define_18.Type.BOOL:
            case define_18.Type.BOOL2:
            case define_18.Type.BOOL3:
            case define_18.Type.BOOL4:
            case define_18.Type.INT:
            case define_18.Type.INT2:
            case define_18.Type.INT3:
            case define_18.Type.INT4:
            case define_18.Type.UINT:
                return Int32Array;
            case define_18.Type.FLOAT:
            case define_18.Type.FLOAT2:
            case define_18.Type.FLOAT3:
            case define_18.Type.FLOAT4:
            case define_18.Type.MAT2:
            case define_18.Type.MAT3:
            case define_18.Type.MAT4:
                return Float32Array;
            default: {
                console.error('Unsupported GLType, convert to TypedArrayConstructor failed.');
                return Float32Array;
            }
        }
    }
    function WebGLTypeToGFXType(glType, gl) {
        switch (glType) {
            case gl.BOOL: return define_18.Type.BOOL;
            case gl.BOOL_VEC2: return define_18.Type.BOOL2;
            case gl.BOOL_VEC3: return define_18.Type.BOOL3;
            case gl.BOOL_VEC4: return define_18.Type.BOOL4;
            case gl.INT: return define_18.Type.INT;
            case gl.INT_VEC2: return define_18.Type.INT2;
            case gl.INT_VEC3: return define_18.Type.INT3;
            case gl.INT_VEC4: return define_18.Type.INT4;
            case gl.UNSIGNED_INT: return define_18.Type.UINT;
            case gl.FLOAT: return define_18.Type.FLOAT;
            case gl.FLOAT_VEC2: return define_18.Type.FLOAT2;
            case gl.FLOAT_VEC3: return define_18.Type.FLOAT3;
            case gl.FLOAT_VEC4: return define_18.Type.FLOAT4;
            case gl.FLOAT_MAT2: return define_18.Type.MAT2;
            case gl.FLOAT_MAT3: return define_18.Type.MAT3;
            case gl.FLOAT_MAT4: return define_18.Type.MAT4;
            case gl.SAMPLER_2D: return define_18.Type.SAMPLER2D;
            case gl.SAMPLER_CUBE: return define_18.Type.SAMPLER_CUBE;
            default: {
                console.error('Unsupported GLType, convert to Type failed.');
                return define_18.Type.UNKNOWN;
            }
        }
    }
    function WebGLGetTypeSize(glType, gl) {
        switch (glType) {
            case gl.BOOL: return 4;
            case gl.BOOL_VEC2: return 8;
            case gl.BOOL_VEC3: return 12;
            case gl.BOOL_VEC4: return 16;
            case gl.INT: return 4;
            case gl.INT_VEC2: return 8;
            case gl.INT_VEC3: return 12;
            case gl.INT_VEC4: return 16;
            case gl.UNSIGNED_INT: return 4;
            case gl.FLOAT: return 4;
            case gl.FLOAT_VEC2: return 8;
            case gl.FLOAT_VEC3: return 12;
            case gl.FLOAT_VEC4: return 16;
            case gl.FLOAT_MAT2: return 16;
            case gl.FLOAT_MAT3: return 36;
            case gl.FLOAT_MAT4: return 64;
            case gl.SAMPLER_2D: return 4;
            case gl.SAMPLER_CUBE: return 4;
            default: {
                console.error('Unsupported GLType, get type failed.');
                return 0;
            }
        }
    }
    function WebGLGetComponentCount(glType, gl) {
        switch (glType) {
            case gl.FLOAT_MAT2: return 2;
            case gl.FLOAT_MAT3: return 3;
            case gl.FLOAT_MAT4: return 4;
            default: {
                return 1;
            }
        }
    }
    function WebGLCmdFuncCreateBuffer(device, gpuBuffer) {
        const { gl } = device;
        const cache = device.stateCache;
        const glUsage = gpuBuffer.memUsage & define_18.MemoryUsageBit.HOST ? gl.DYNAMIC_DRAW : gl.STATIC_DRAW;
        if (gpuBuffer.usage & define_18.BufferUsageBit.VERTEX) {
            gpuBuffer.glTarget = gl.ARRAY_BUFFER;
            const glBuffer = gl.createBuffer();
            if (glBuffer) {
                gpuBuffer.glBuffer = glBuffer;
                if (gpuBuffer.size > 0) {
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            device.extensions.OES_vertex_array_object.bindVertexArrayOES(null);
                            cache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    if (device.stateCache.glArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ARRAY_BUFFER, gpuBuffer.glBuffer);
                        device.stateCache.glArrayBuffer = gpuBuffer.glBuffer;
                    }
                    gl.bufferData(gl.ARRAY_BUFFER, gpuBuffer.size, glUsage);
                    gl.bindBuffer(gl.ARRAY_BUFFER, null);
                    device.stateCache.glArrayBuffer = null;
                }
            }
        }
        else if (gpuBuffer.usage & define_18.BufferUsageBit.INDEX) {
            gpuBuffer.glTarget = gl.ELEMENT_ARRAY_BUFFER;
            const glBuffer = gl.createBuffer();
            if (glBuffer) {
                gpuBuffer.glBuffer = glBuffer;
                if (gpuBuffer.size > 0) {
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            device.extensions.OES_vertex_array_object.bindVertexArrayOES(null);
                            cache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    if (device.stateCache.glElementArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
                        device.stateCache.glElementArrayBuffer = gpuBuffer.glBuffer;
                    }
                    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.size, glUsage);
                    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
                    device.stateCache.glElementArrayBuffer = null;
                }
            }
        }
        else if (gpuBuffer.usage & define_18.BufferUsageBit.UNIFORM) {
            gpuBuffer.glTarget = gl.NONE;
            if (gpuBuffer.buffer) {
                gpuBuffer.vf32 = new Float32Array(gpuBuffer.buffer.buffer);
            }
        }
        else if (gpuBuffer.usage & define_18.BufferUsageBit.INDIRECT) {
            gpuBuffer.glTarget = gl.NONE;
        }
        else if (gpuBuffer.usage & define_18.BufferUsageBit.TRANSFER_DST) {
            gpuBuffer.glTarget = gl.NONE;
        }
        else if (gpuBuffer.usage & define_18.BufferUsageBit.TRANSFER_SRC) {
            gpuBuffer.glTarget = gl.NONE;
        }
        else {
            console.error('Unsupported BufferType, create buffer failed.');
            gpuBuffer.glTarget = gl.NONE;
        }
    }
    exports_27("WebGLCmdFuncCreateBuffer", WebGLCmdFuncCreateBuffer);
    function WebGLCmdFuncDestroyBuffer(device, gpuBuffer) {
        const { gl } = device;
        const cache = device.stateCache;
        if (gpuBuffer.glBuffer) {
            // Firefox 75+ implicitly unbind whatever buffer there was on the slot sometimes
            // can be reproduced in the static batching scene at https://github.com/cocos-creator/test-cases-3d
            switch (gpuBuffer.glTarget) {
                case gl.ARRAY_BUFFER:
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            device.extensions.OES_vertex_array_object.bindVertexArrayOES(null);
                            device.stateCache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    gl.bindBuffer(gl.ARRAY_BUFFER, null);
                    device.stateCache.glArrayBuffer = null;
                    break;
                case gl.ELEMENT_ARRAY_BUFFER:
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            device.extensions.OES_vertex_array_object.bindVertexArrayOES(null);
                            device.stateCache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
                    device.stateCache.glElementArrayBuffer = null;
                    break;
                default:
            }
            gl.deleteBuffer(gpuBuffer.glBuffer);
            gpuBuffer.glBuffer = null;
        }
    }
    exports_27("WebGLCmdFuncDestroyBuffer", WebGLCmdFuncDestroyBuffer);
    function WebGLCmdFuncResizeBuffer(device, gpuBuffer) {
        const { gl } = device;
        const cache = device.stateCache;
        const glUsage = gpuBuffer.memUsage & define_18.MemoryUsageBit.HOST ? gl.DYNAMIC_DRAW : gl.STATIC_DRAW;
        if (gpuBuffer.usage & define_18.BufferUsageBit.VERTEX) {
            if (device.extensions.useVAO) {
                if (cache.glVAO) {
                    device.extensions.OES_vertex_array_object.bindVertexArrayOES(null);
                    cache.glVAO = null;
                }
            }
            gfxStateCache.gpuInputAssembler = null;
            if (device.stateCache.glArrayBuffer !== gpuBuffer.glBuffer) {
                gl.bindBuffer(gl.ARRAY_BUFFER, gpuBuffer.glBuffer);
            }
            if (gpuBuffer.buffer) {
                gl.bufferData(gl.ARRAY_BUFFER, gpuBuffer.buffer, glUsage);
            }
            else {
                gl.bufferData(gl.ARRAY_BUFFER, gpuBuffer.size, glUsage);
            }
            gl.bindBuffer(gl.ARRAY_BUFFER, null);
            device.stateCache.glArrayBuffer = null;
        }
        else if (gpuBuffer.usage & define_18.BufferUsageBit.INDEX) {
            if (device.extensions.useVAO) {
                if (cache.glVAO) {
                    device.extensions.OES_vertex_array_object.bindVertexArrayOES(null);
                    cache.glVAO = null;
                }
            }
            gfxStateCache.gpuInputAssembler = null;
            if (device.stateCache.glElementArrayBuffer !== gpuBuffer.glBuffer) {
                gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
            }
            if (gpuBuffer.buffer) {
                gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.buffer, glUsage);
            }
            else {
                gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.size, glUsage);
            }
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
            device.stateCache.glElementArrayBuffer = null;
        }
        else if (gpuBuffer.usage & define_18.BufferUsageBit.UNIFORM) {
            if (gpuBuffer.buffer) {
                gpuBuffer.vf32 = new Float32Array(gpuBuffer.buffer.buffer);
            }
        }
        else if ((gpuBuffer.usage & define_18.BufferUsageBit.INDIRECT)
            || (gpuBuffer.usage & define_18.BufferUsageBit.TRANSFER_DST)
            || (gpuBuffer.usage & define_18.BufferUsageBit.TRANSFER_SRC)) {
            gpuBuffer.glTarget = gl.NONE;
        }
        else {
            console.error('Unsupported BufferType, create buffer failed.');
            gpuBuffer.glTarget = gl.NONE;
        }
    }
    exports_27("WebGLCmdFuncResizeBuffer", WebGLCmdFuncResizeBuffer);
    function WebGLCmdFuncUpdateBuffer(device, gpuBuffer, buffer, offset, size) {
        if (gpuBuffer.usage & define_18.BufferUsageBit.UNIFORM) {
            if (ArrayBuffer.isView(buffer)) {
                gpuBuffer.vf32.set(buffer, offset / Float32Array.BYTES_PER_ELEMENT);
            }
            else {
                gpuBuffer.vf32.set(new Float32Array(buffer), offset / Float32Array.BYTES_PER_ELEMENT);
            }
        }
        else if (gpuBuffer.usage & define_18.BufferUsageBit.INDIRECT) {
            gpuBuffer.indirects.clearDraws();
            const drawInfos = buffer.drawInfos;
            for (let i = 0; i < drawInfos.length; ++i) {
                gpuBuffer.indirects.setDrawInfo(offset + i, drawInfos[i]);
            }
        }
        else {
            const buff = buffer;
            const { gl } = device;
            const cache = device.stateCache;
            switch (gpuBuffer.glTarget) {
                case gl.ARRAY_BUFFER: {
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            device.extensions.OES_vertex_array_object.bindVertexArrayOES(null);
                            cache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    if (device.stateCache.glArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ARRAY_BUFFER, gpuBuffer.glBuffer);
                        device.stateCache.glArrayBuffer = gpuBuffer.glBuffer;
                    }
                    break;
                }
                case gl.ELEMENT_ARRAY_BUFFER: {
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            device.extensions.OES_vertex_array_object.bindVertexArrayOES(null);
                            cache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    if (device.stateCache.glElementArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
                        device.stateCache.glElementArrayBuffer = gpuBuffer.glBuffer;
                    }
                    break;
                }
                default: {
                    console.error('Unsupported BufferType, update buffer failed.');
                    return;
                }
            }
            if (size === buff.byteLength) {
                gl.bufferSubData(gpuBuffer.glTarget, offset, buff);
            }
            else {
                gl.bufferSubData(gpuBuffer.glTarget, offset, buff.slice(0, size));
            }
        }
    }
    exports_27("WebGLCmdFuncUpdateBuffer", WebGLCmdFuncUpdateBuffer);
    function WebGLCmdFuncCreateTexture(device, gpuTexture) {
        const { gl } = device;
        gpuTexture.glFormat = gpuTexture.glInternalFmt = GFXFormatToWebGLFormat(gpuTexture.format, gl);
        gpuTexture.glType = GFXFormatToWebGLType(gpuTexture.format, gl);
        let w = gpuTexture.width;
        let h = gpuTexture.height;
        switch (gpuTexture.type) {
            case define_18.TextureType.TEX2D: {
                gpuTexture.glTarget = gl.TEXTURE_2D;
                if (gpuTexture.isSwapchainTexture)
                    break;
                const maxSize = Math.max(w, h);
                if (maxSize > device.capabilities.maxTextureSize) {
                    debug_1.errorID(9100, maxSize, device.capabilities.maxTextureSize);
                }
                if (!device.extensions.WEBGL_depth_texture && define_18.FormatInfos[gpuTexture.format].hasDepth) {
                    gpuTexture.glInternalFmt = GFXFormatToWebGLInternalFormat(gpuTexture.format, gl);
                    gpuTexture.glRenderbuffer = gl.createRenderbuffer();
                    if (gpuTexture.size > 0) {
                        if (device.stateCache.glRenderbuffer !== gpuTexture.glRenderbuffer) {
                            gl.bindRenderbuffer(gl.RENDERBUFFER, gpuTexture.glRenderbuffer);
                            device.stateCache.glRenderbuffer = gpuTexture.glRenderbuffer;
                        }
                        gl.renderbufferStorage(gl.RENDERBUFFER, gpuTexture.glInternalFmt, w, h);
                    }
                }
                else {
                    gpuTexture.glTexture = gl.createTexture();
                    if (gpuTexture.size > 0) {
                        const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
                        if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                            gl.bindTexture(gl.TEXTURE_2D, gpuTexture.glTexture);
                            glTexUnit.glTexture = gpuTexture.glTexture;
                        }
                        if (define_18.FormatInfos[gpuTexture.format].isCompressed) {
                            for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                                const imgSize = define_18.FormatSize(gpuTexture.format, w, h, 1);
                                const view = new Uint8Array(imgSize);
                                gl.compressedTexImage2D(gl.TEXTURE_2D, i, gpuTexture.glInternalFmt, w, h, 0, view);
                                w = Math.max(1, w >> 1);
                                h = Math.max(1, h >> 1);
                            }
                        }
                        else {
                            for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                                gl.texImage2D(gl.TEXTURE_2D, i, gpuTexture.glInternalFmt, w, h, 0, gpuTexture.glFormat, gpuTexture.glType, null);
                                w = Math.max(1, w >> 1);
                                h = Math.max(1, h >> 1);
                            }
                        }
                        if (gpuTexture.isPowerOf2) {
                            gpuTexture.glWrapS = gl.REPEAT;
                            gpuTexture.glWrapT = gl.REPEAT;
                        }
                        else {
                            gpuTexture.glWrapS = gl.CLAMP_TO_EDGE;
                            gpuTexture.glWrapT = gl.CLAMP_TO_EDGE;
                        }
                        gpuTexture.glMinFilter = gl.LINEAR;
                        gpuTexture.glMagFilter = gl.LINEAR;
                        gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_WRAP_S, gpuTexture.glWrapS);
                        gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_WRAP_T, gpuTexture.glWrapT);
                        gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_MIN_FILTER, gpuTexture.glMinFilter);
                        gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_MAG_FILTER, gpuTexture.glMagFilter);
                    }
                }
                break;
            }
            case define_18.TextureType.CUBE: {
                gpuTexture.glTarget = gl.TEXTURE_CUBE_MAP;
                const maxSize = Math.max(w, h);
                if (maxSize > device.capabilities.maxCubeMapTextureSize) {
                    debug_1.errorID(9100, maxSize, device.capabilities.maxTextureSize);
                }
                gpuTexture.glTexture = gl.createTexture();
                if (gpuTexture.size > 0) {
                    const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
                    if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                        gl.bindTexture(gl.TEXTURE_CUBE_MAP, gpuTexture.glTexture);
                        glTexUnit.glTexture = gpuTexture.glTexture;
                    }
                    if (define_18.FormatInfos[gpuTexture.format].isCompressed) {
                        for (let f = 0; f < 6; ++f) {
                            w = gpuTexture.width;
                            h = gpuTexture.height;
                            for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                                const imgSize = define_18.FormatSize(gpuTexture.format, w, h, 1);
                                const view = new Uint8Array(imgSize);
                                gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, i, gpuTexture.glInternalFmt, w, h, 0, view);
                                w = Math.max(1, w >> 1);
                                h = Math.max(1, h >> 1);
                            }
                        }
                    }
                    else {
                        for (let f = 0; f < 6; ++f) {
                            w = gpuTexture.width;
                            h = gpuTexture.height;
                            for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                                gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, i, gpuTexture.glInternalFmt, w, h, 0, gpuTexture.glFormat, gpuTexture.glType, null);
                                w = Math.max(1, w >> 1);
                                h = Math.max(1, h >> 1);
                            }
                        }
                    }
                    if (gpuTexture.isPowerOf2) {
                        gpuTexture.glWrapS = gl.REPEAT;
                        gpuTexture.glWrapT = gl.REPEAT;
                    }
                    else {
                        gpuTexture.glWrapS = gl.CLAMP_TO_EDGE;
                        gpuTexture.glWrapT = gl.CLAMP_TO_EDGE;
                    }
                    gpuTexture.glMinFilter = gl.LINEAR;
                    gpuTexture.glMagFilter = gl.LINEAR;
                    gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_WRAP_S, gpuTexture.glWrapS);
                    gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_WRAP_T, gpuTexture.glWrapT);
                    gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_MIN_FILTER, gpuTexture.glMinFilter);
                    gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_MAG_FILTER, gpuTexture.glMagFilter);
                }
                break;
            }
            default: {
                console.error('Unsupported TextureType, create texture failed.');
                gpuTexture.type = define_18.TextureType.TEX2D;
                gpuTexture.glTarget = gl.TEXTURE_2D;
            }
        }
    }
    exports_27("WebGLCmdFuncCreateTexture", WebGLCmdFuncCreateTexture);
    function WebGLCmdFuncDestroyTexture(device, gpuTexture) {
        if (gpuTexture.glTexture) {
            device.gl.deleteTexture(gpuTexture.glTexture);
            gpuTexture.glTexture = null;
        }
        if (gpuTexture.glRenderbuffer) {
            device.gl.deleteRenderbuffer(gpuTexture.glRenderbuffer);
            gpuTexture.glRenderbuffer = null;
        }
    }
    exports_27("WebGLCmdFuncDestroyTexture", WebGLCmdFuncDestroyTexture);
    function WebGLCmdFuncResizeTexture(device, gpuTexture) {
        if (!gpuTexture.size)
            return;
        const { gl } = device;
        let w = gpuTexture.width;
        let h = gpuTexture.height;
        switch (gpuTexture.type) {
            case define_18.TextureType.TEX2D: {
                gpuTexture.glTarget = gl.TEXTURE_2D;
                const maxSize = Math.max(w, h);
                if (maxSize > device.capabilities.maxTextureSize) {
                    debug_1.errorID(9100, maxSize, device.capabilities.maxTextureSize);
                }
                if (gpuTexture.glRenderbuffer) {
                    if (device.stateCache.glRenderbuffer !== gpuTexture.glRenderbuffer) {
                        gl.bindRenderbuffer(gl.RENDERBUFFER, gpuTexture.glRenderbuffer);
                        device.stateCache.glRenderbuffer = gpuTexture.glRenderbuffer;
                    }
                    gl.renderbufferStorage(gl.RENDERBUFFER, gpuTexture.glInternalFmt, w, h);
                }
                else if (gpuTexture.glTexture) {
                    const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
                    if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                        gl.bindTexture(gl.TEXTURE_2D, gpuTexture.glTexture);
                        glTexUnit.glTexture = gpuTexture.glTexture;
                    }
                    if (define_18.FormatInfos[gpuTexture.format].isCompressed) {
                        for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                            const imgSize = define_18.FormatSize(gpuTexture.format, w, h, 1);
                            const view = new Uint8Array(imgSize);
                            gl.compressedTexImage2D(gl.TEXTURE_2D, i, gpuTexture.glInternalFmt, w, h, 0, view);
                            w = Math.max(1, w >> 1);
                            h = Math.max(1, h >> 1);
                        }
                    }
                    else {
                        for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                            gl.texImage2D(gl.TEXTURE_2D, i, gpuTexture.glInternalFmt, w, h, 0, gpuTexture.glFormat, gpuTexture.glType, null);
                            w = Math.max(1, w >> 1);
                            h = Math.max(1, h >> 1);
                        }
                    }
                }
                break;
            }
            case define_18.TextureType.CUBE: {
                gpuTexture.glTarget = gl.TEXTURE_CUBE_MAP;
                const maxSize = Math.max(w, h);
                if (maxSize > device.capabilities.maxCubeMapTextureSize) {
                    debug_1.errorID(9100, maxSize, device.capabilities.maxTextureSize);
                }
                const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
                if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                    gl.bindTexture(gl.TEXTURE_CUBE_MAP, gpuTexture.glTexture);
                    glTexUnit.glTexture = gpuTexture.glTexture;
                }
                if (define_18.FormatInfos[gpuTexture.format].isCompressed) {
                    for (let f = 0; f < 6; ++f) {
                        w = gpuTexture.width;
                        h = gpuTexture.height;
                        for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                            const imgSize = define_18.FormatSize(gpuTexture.format, w, h, 1);
                            const view = new Uint8Array(imgSize);
                            gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, i, gpuTexture.glInternalFmt, w, h, 0, view);
                            w = Math.max(1, w >> 1);
                            h = Math.max(1, h >> 1);
                        }
                    }
                }
                else {
                    for (let f = 0; f < 6; ++f) {
                        w = gpuTexture.width;
                        h = gpuTexture.height;
                        for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                            gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, i, gpuTexture.glInternalFmt, w, h, 0, gpuTexture.glFormat, gpuTexture.glType, null);
                            w = Math.max(1, w >> 1);
                            h = Math.max(1, h >> 1);
                        }
                    }
                }
                break;
            }
            default: {
                console.error('Unsupported TextureType, create texture failed.');
                gpuTexture.type = define_18.TextureType.TEX2D;
                gpuTexture.glTarget = gl.TEXTURE_2D;
            }
        }
    }
    exports_27("WebGLCmdFuncResizeTexture", WebGLCmdFuncResizeTexture);
    function WebGLCmdFuncCreateFramebuffer(device, gpuFramebuffer) {
        for (let i = 0; i < gpuFramebuffer.gpuColorTextures.length; ++i) {
            const tex = gpuFramebuffer.gpuColorTextures[i];
            if (tex.isSwapchainTexture) {
                gpuFramebuffer.isOffscreen = false;
                return;
            }
        }
        const { gl } = device;
        const attachments = [];
        const glFramebuffer = gl.createFramebuffer();
        if (glFramebuffer) {
            gpuFramebuffer.glFramebuffer = glFramebuffer;
            if (device.stateCache.glFramebuffer !== gpuFramebuffer.glFramebuffer) {
                gl.bindFramebuffer(gl.FRAMEBUFFER, gpuFramebuffer.glFramebuffer);
            }
            for (let i = 0; i < gpuFramebuffer.gpuColorTextures.length; ++i) {
                const gpuTexture = gpuFramebuffer.gpuColorTextures[i];
                if (gpuTexture) {
                    if (gpuTexture.glTexture) {
                        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0 + i, gpuTexture.glTarget, gpuTexture.glTexture, 0); // level must be 0
                    }
                    else {
                        gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0 + i, gl.RENDERBUFFER, gpuTexture.glRenderbuffer);
                    }
                    attachments.push(gl.COLOR_ATTACHMENT0 + i);
                    gpuFramebuffer.width = Math.min(gpuFramebuffer.width, gpuTexture.width);
                    gpuFramebuffer.height = Math.min(gpuFramebuffer.height, gpuTexture.height);
                }
            }
            const dst = gpuFramebuffer.gpuDepthStencilTexture;
            if (dst) {
                const glAttachment = define_18.FormatInfos[dst.format].hasStencil ? gl.DEPTH_STENCIL_ATTACHMENT : gl.DEPTH_ATTACHMENT;
                if (dst.glTexture) {
                    gl.framebufferTexture2D(gl.FRAMEBUFFER, glAttachment, dst.glTarget, dst.glTexture, 0); // level must be 0
                }
                else {
                    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, glAttachment, gl.RENDERBUFFER, dst.glRenderbuffer);
                }
                gpuFramebuffer.width = Math.min(gpuFramebuffer.width, dst.width);
                gpuFramebuffer.height = Math.min(gpuFramebuffer.height, dst.height);
            }
            if (device.extensions.WEBGL_draw_buffers) {
                device.extensions.WEBGL_draw_buffers.drawBuffersWEBGL(attachments);
            }
            const status = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
            if (status !== gl.FRAMEBUFFER_COMPLETE) {
                switch (status) {
                    case gl.FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
                        console.error('glCheckFramebufferStatus() - FRAMEBUFFER_INCOMPLETE_ATTACHMENT');
                        break;
                    }
                    case gl.FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
                        console.error('glCheckFramebufferStatus() - FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT');
                        break;
                    }
                    case gl.FRAMEBUFFER_INCOMPLETE_DIMENSIONS: {
                        console.error('glCheckFramebufferStatus() - FRAMEBUFFER_INCOMPLETE_DIMENSIONS');
                        break;
                    }
                    case gl.FRAMEBUFFER_UNSUPPORTED: {
                        console.error('glCheckFramebufferStatus() - FRAMEBUFFER_UNSUPPORTED');
                        break;
                    }
                    default:
                }
            }
            if (device.stateCache.glFramebuffer !== gpuFramebuffer.glFramebuffer) {
                gl.bindFramebuffer(gl.FRAMEBUFFER, device.stateCache.glFramebuffer);
            }
        }
    }
    exports_27("WebGLCmdFuncCreateFramebuffer", WebGLCmdFuncCreateFramebuffer);
    function WebGLCmdFuncDestroyFramebuffer(device, gpuFramebuffer) {
        if (gpuFramebuffer.glFramebuffer) {
            device.gl.deleteFramebuffer(gpuFramebuffer.glFramebuffer);
            gpuFramebuffer.glFramebuffer = null;
        }
    }
    exports_27("WebGLCmdFuncDestroyFramebuffer", WebGLCmdFuncDestroyFramebuffer);
    function WebGLCmdFuncCreateShader(device, gpuShader) {
        const { gl } = device;
        for (let k = 0; k < gpuShader.gpuStages.length; k++) {
            const gpuStage = gpuShader.gpuStages[k];
            let glShaderType = 0;
            let shaderTypeStr = '';
            let lineNumber = 1;
            switch (gpuStage.type) {
                case define_18.ShaderStageFlagBit.VERTEX: {
                    shaderTypeStr = 'VertexShader';
                    glShaderType = gl.VERTEX_SHADER;
                    break;
                }
                case define_18.ShaderStageFlagBit.FRAGMENT: {
                    shaderTypeStr = 'FragmentShader';
                    glShaderType = gl.FRAGMENT_SHADER;
                    break;
                }
                default: {
                    console.error('Unsupported ShaderType.');
                    return;
                }
            }
            const glShader = gl.createShader(glShaderType);
            if (glShader) {
                gpuStage.glShader = glShader;
                gl.shaderSource(gpuStage.glShader, gpuStage.source);
                gl.compileShader(gpuStage.glShader);
                if (!gl.getShaderParameter(gpuStage.glShader, gl.COMPILE_STATUS)) {
                    console.error(`${shaderTypeStr} in '${gpuShader.name}' compilation failed.`);
                    console.error('Shader source dump:', gpuStage.source.replace(/^|\n/g, () => `\n${lineNumber++} `));
                    console.error(gl.getShaderInfoLog(gpuStage.glShader));
                    for (let l = 0; l < gpuShader.gpuStages.length; l++) {
                        const stage = gpuShader.gpuStages[k];
                        if (stage.glShader) {
                            gl.deleteShader(stage.glShader);
                            stage.glShader = null;
                        }
                    }
                    return;
                }
            }
        }
        const glProgram = gl.createProgram();
        if (!glProgram) {
            return;
        }
        gpuShader.glProgram = glProgram;
        // link program
        for (let k = 0; k < gpuShader.gpuStages.length; k++) {
            const gpuStage = gpuShader.gpuStages[k];
            gl.attachShader(gpuShader.glProgram, gpuStage.glShader);
        }
        gl.linkProgram(gpuShader.glProgram);
        // detach & delete immediately
        if (device.extensions.destroyShadersImmediately) {
            for (let k = 0; k < gpuShader.gpuStages.length; k++) {
                const gpuStage = gpuShader.gpuStages[k];
                if (gpuStage.glShader) {
                    gl.detachShader(gpuShader.glProgram, gpuStage.glShader);
                    gl.deleteShader(gpuStage.glShader);
                    gpuStage.glShader = null;
                }
            }
        }
        if (gl.getProgramParameter(gpuShader.glProgram, gl.LINK_STATUS)) {
            debug_1.debug(`Shader '${gpuShader.name}' compilation succeeded.`);
        }
        else {
            console.error(`Failed to link shader '${gpuShader.name}'.`);
            console.error(gl.getProgramInfoLog(gpuShader.glProgram));
            return;
        }
        // parse inputs
        const activeAttribCount = gl.getProgramParameter(gpuShader.glProgram, gl.ACTIVE_ATTRIBUTES);
        gpuShader.glInputs = new Array(activeAttribCount);
        for (let i = 0; i < activeAttribCount; ++i) {
            const attribInfo = gl.getActiveAttrib(gpuShader.glProgram, i);
            if (attribInfo) {
                let varName;
                const nameOffset = attribInfo.name.indexOf('[');
                if (nameOffset !== -1) {
                    varName = attribInfo.name.substr(0, nameOffset);
                }
                else {
                    varName = attribInfo.name;
                }
                const glLoc = gl.getAttribLocation(gpuShader.glProgram, varName);
                const type = WebGLTypeToGFXType(attribInfo.type, gl);
                const stride = WebGLGetTypeSize(attribInfo.type, gl);
                gpuShader.glInputs[i] = {
                    binding: glLoc,
                    name: varName,
                    type,
                    stride,
                    count: attribInfo.size,
                    size: stride * attribInfo.size,
                    glType: attribInfo.type,
                    glLoc,
                };
            }
        }
        // create uniform blocks
        if (gpuShader.blocks.length > 0) {
            gpuShader.glBlocks = new Array(gpuShader.blocks.length);
            for (let i = 0; i < gpuShader.blocks.length; ++i) {
                const block = gpuShader.blocks[i];
                const glBlock = {
                    set: block.set,
                    binding: block.binding,
                    name: block.name,
                    size: 0,
                    glUniforms: new Array(block.members.length),
                    glActiveUniforms: [],
                };
                gpuShader.glBlocks[i] = glBlock;
                for (let u = 0; u < block.members.length; ++u) {
                    const uniform = block.members[u];
                    const glType = GFXTypeToWebGLType(uniform.type, gl);
                    const stride = WebGLGetTypeSize(glType, gl);
                    const size = stride * uniform.count;
                    glBlock.glUniforms[u] = {
                        binding: -1,
                        name: uniform.name,
                        type: uniform.type,
                        stride,
                        count: uniform.count,
                        size,
                        offset: 0,
                        glType,
                        glLoc: null,
                        array: null,
                    };
                }
            }
        }
        // WebGL doesn't support Framebuffer Fetch
        for (let i = 0; i < gpuShader.subpassInputs.length; ++i) {
            const subpassInput = gpuShader.subpassInputs[i];
            gpuShader.samplerTextures.push(new define_18.UniformSamplerTexture(subpassInput.set, subpassInput.binding, subpassInput.name, define_18.Type.SAMPLER2D, subpassInput.count));
        }
        // create uniform sampler textures
        if (gpuShader.samplerTextures.length > 0) {
            gpuShader.glSamplerTextures = new Array(gpuShader.samplerTextures.length);
            for (let i = 0; i < gpuShader.samplerTextures.length; ++i) {
                const sampler = gpuShader.samplerTextures[i];
                gpuShader.glSamplerTextures[i] = {
                    set: sampler.set,
                    binding: sampler.binding,
                    name: sampler.name,
                    type: sampler.type,
                    count: sampler.count,
                    units: [],
                    glUnits: null,
                    glType: GFXTypeToWebGLType(sampler.type, gl),
                    glLoc: null,
                };
            }
        }
        // parse uniforms
        const activeUniformCount = gl.getProgramParameter(gpuShader.glProgram, gl.ACTIVE_UNIFORMS);
        for (let i = 0; i < activeUniformCount; ++i) {
            const uniformInfo = gl.getActiveUniform(gpuShader.glProgram, i);
            if (uniformInfo) {
                const isSampler = (uniformInfo.type === gl.SAMPLER_2D)
                    || (uniformInfo.type === gl.SAMPLER_CUBE);
                if (!isSampler) {
                    const glLoc = gl.getUniformLocation(gpuShader.glProgram, uniformInfo.name);
                    if (device.extensions.isLocationActive(glLoc)) {
                        let varName;
                        const nameOffset = uniformInfo.name.indexOf('[');
                        if (nameOffset !== -1) {
                            varName = uniformInfo.name.substr(0, nameOffset);
                        }
                        else {
                            varName = uniformInfo.name;
                        }
                        // build uniform block mapping
                        for (let j = 0; j < gpuShader.glBlocks.length; j++) {
                            const glBlock = gpuShader.glBlocks[j];
                            for (let k = 0; k < glBlock.glUniforms.length; k++) {
                                const glUniform = glBlock.glUniforms[k];
                                if (glUniform.name === varName) {
                                    glUniform.glLoc = glLoc;
                                    glUniform.count = uniformInfo.size;
                                    glUniform.size = glUniform.stride * glUniform.count;
                                    glUniform.array = new (GFXTypeToTypedArrayCtor(glUniform.type))(glUniform.size / 4);
                                    glBlock.glActiveUniforms.push(glUniform);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        // calculate offset & size
        // WARNING: we can't handle inactive uniform arrays with wrong input sizes
        // and there is no way to detect that for now
        for (let j = 0; j < gpuShader.glBlocks.length; j++) {
            const glBlock = gpuShader.glBlocks[j];
            for (let k = 0; k < glBlock.glUniforms.length; k++) {
                const glUniform = glBlock.glUniforms[k];
                glUniform.offset = glBlock.size / 4;
                glBlock.size += glUniform.size;
            }
        }
        // texture unit index mapping optimization
        const glActiveSamplers = [];
        const glActiveSamplerLocations = [];
        const { bindingMappingInfo } = device;
        const { texUnitCacheMap } = device.stateCache;
        let flexibleSetBaseOffset = 0;
        for (let i = 0; i < gpuShader.blocks.length; ++i) {
            if (gpuShader.blocks[i].set === bindingMappingInfo.flexibleSet) {
                flexibleSetBaseOffset++;
            }
        }
        let arrayOffset = 0;
        for (let i = 0; i < gpuShader.samplerTextures.length; ++i) {
            const sampler = gpuShader.samplerTextures[i];
            const glLoc = gl.getUniformLocation(gpuShader.glProgram, sampler.name);
            if (device.extensions.isLocationActive(glLoc)) {
                glActiveSamplers.push(gpuShader.glSamplerTextures[i]);
                glActiveSamplerLocations.push(glLoc);
            }
            if (texUnitCacheMap[sampler.name] === undefined) {
                let binding = sampler.binding + bindingMappingInfo.samplerOffsets[sampler.set] + arrayOffset;
                if (sampler.set === bindingMappingInfo.flexibleSet) {
                    binding -= flexibleSetBaseOffset;
                }
                texUnitCacheMap[sampler.name] = binding % device.capabilities.maxTextureUnits;
                arrayOffset += sampler.count - 1;
            }
        }
        if (glActiveSamplers.length) {
            const usedTexUnits = [];
            // try to reuse existing mappings first
            for (let i = 0; i < glActiveSamplers.length; ++i) {
                const glSampler = glActiveSamplers[i];
                let cachedUnit = texUnitCacheMap[glSampler.name];
                if (cachedUnit !== undefined) {
                    glSampler.glLoc = glActiveSamplerLocations[i];
                    for (let t = 0; t < glSampler.count; ++t) {
                        while (usedTexUnits[cachedUnit]) {
                            cachedUnit = (cachedUnit + 1) % device.capabilities.maxTextureUnits;
                        }
                        glSampler.units.push(cachedUnit);
                        usedTexUnits[cachedUnit] = true;
                    }
                }
            }
            // fill in the rest sequencially
            let unitIdx = 0;
            for (let i = 0; i < glActiveSamplers.length; ++i) {
                const glSampler = glActiveSamplers[i];
                if (!device.extensions.isLocationActive(glSampler.glLoc)) {
                    glSampler.glLoc = glActiveSamplerLocations[i];
                    for (let t = 0; t < glSampler.count; ++t) {
                        while (usedTexUnits[unitIdx]) {
                            unitIdx = (unitIdx + 1) % device.capabilities.maxTextureUnits;
                        }
                        if (texUnitCacheMap[glSampler.name] === undefined) {
                            texUnitCacheMap[glSampler.name] = unitIdx;
                        }
                        glSampler.units.push(unitIdx);
                        usedTexUnits[unitIdx] = true;
                    }
                }
            }
            if (device.stateCache.glProgram !== gpuShader.glProgram) {
                gl.useProgram(gpuShader.glProgram);
            }
            for (let i = 0; i < glActiveSamplers.length; i++) {
                const glSampler = glActiveSamplers[i];
                glSampler.glUnits = new Int32Array(glSampler.units);
                gl.uniform1iv(glSampler.glLoc, glSampler.glUnits);
            }
            if (device.stateCache.glProgram !== gpuShader.glProgram) {
                gl.useProgram(device.stateCache.glProgram);
            }
        }
        // strip out the inactive ones
        for (let i = 0; i < gpuShader.glBlocks.length;) {
            if (gpuShader.glBlocks[i].glActiveUniforms.length) {
                i++;
            }
            else {
                gpuShader.glBlocks[i] = gpuShader.glBlocks[gpuShader.glBlocks.length - 1];
                gpuShader.glBlocks.length--;
            }
        }
        gpuShader.glSamplerTextures = glActiveSamplers;
    }
    exports_27("WebGLCmdFuncCreateShader", WebGLCmdFuncCreateShader);
    function WebGLCmdFuncDestroyShader(device, gpuShader) {
        if (gpuShader.glProgram) {
            const { gl } = device;
            if (!device.extensions.destroyShadersImmediately) {
                for (let k = 0; k < gpuShader.gpuStages.length; k++) {
                    const gpuStage = gpuShader.gpuStages[k];
                    if (gpuStage.glShader) {
                        gl.detachShader(gpuShader.glProgram, gpuStage.glShader);
                        gl.deleteShader(gpuStage.glShader);
                        gpuStage.glShader = null;
                    }
                }
            }
            gl.deleteProgram(gpuShader.glProgram);
            gpuShader.glProgram = null;
        }
    }
    exports_27("WebGLCmdFuncDestroyShader", WebGLCmdFuncDestroyShader);
    function WebGLCmdFuncCreateInputAssember(device, gpuInputAssembler) {
        const { gl } = device;
        gpuInputAssembler.glAttribs = new Array(gpuInputAssembler.attributes.length);
        const offsets = [0, 0, 0, 0, 0, 0, 0, 0];
        for (let i = 0; i < gpuInputAssembler.attributes.length; ++i) {
            const attrib = gpuInputAssembler.attributes[i];
            const stream = attrib.stream !== undefined ? attrib.stream : 0;
            const gpuBuffer = gpuInputAssembler.gpuVertexBuffers[stream];
            const glType = GFXFormatToWebGLType(attrib.format, gl);
            const { size } = define_18.FormatInfos[attrib.format];
            gpuInputAssembler.glAttribs[i] = {
                name: attrib.name,
                glBuffer: gpuBuffer.glBuffer,
                glType,
                size,
                count: define_18.FormatInfos[attrib.format].count,
                stride: gpuBuffer.stride,
                componentCount: WebGLGetComponentCount(glType, gl),
                isNormalized: (attrib.isNormalized !== undefined ? attrib.isNormalized : false),
                isInstanced: (attrib.isInstanced !== undefined ? attrib.isInstanced : false),
                offset: offsets[stream],
            };
            offsets[stream] += size;
        }
    }
    exports_27("WebGLCmdFuncCreateInputAssember", WebGLCmdFuncCreateInputAssember);
    function WebGLCmdFuncDestroyInputAssembler(device, gpuInputAssembler) {
        const it = gpuInputAssembler.glVAOs.values();
        let res = it.next();
        while (!res.done) {
            device.extensions.OES_vertex_array_object.deleteVertexArrayOES(res.value);
            res = it.next();
        }
        gpuInputAssembler.glVAOs.clear();
    }
    exports_27("WebGLCmdFuncDestroyInputAssembler", WebGLCmdFuncDestroyInputAssembler);
    function WebGLCmdFuncBeginRenderPass(device, gpuRenderPass, gpuFramebuffer, renderArea, clearColors, clearDepth, clearStencil) {
        const { gl } = device;
        const cache = device.stateCache;
        let clears = 0;
        if (gpuFramebuffer && gpuRenderPass) {
            if (cache.glFramebuffer !== gpuFramebuffer.glFramebuffer) {
                gl.bindFramebuffer(gl.FRAMEBUFFER, gpuFramebuffer.glFramebuffer);
                cache.glFramebuffer = gpuFramebuffer.glFramebuffer;
            }
            if (cache.viewport.left !== renderArea.x
                || cache.viewport.top !== renderArea.y
                || cache.viewport.width !== renderArea.width
                || cache.viewport.height !== renderArea.height) {
                gl.viewport(renderArea.x, renderArea.y, renderArea.width, renderArea.height);
                cache.viewport.left = renderArea.x;
                cache.viewport.top = renderArea.y;
                cache.viewport.width = renderArea.width;
                cache.viewport.height = renderArea.height;
            }
            if (cache.scissorRect.x !== 0
                || cache.scissorRect.y !== 0
                || cache.scissorRect.width !== gpuFramebuffer.width
                || cache.scissorRect.height !== gpuFramebuffer.height) {
                gl.scissor(0, 0, gpuFramebuffer.width, gpuFramebuffer.height);
                cache.scissorRect.x = 0;
                cache.scissorRect.y = 0;
                cache.scissorRect.width = gpuFramebuffer.width;
                cache.scissorRect.height = gpuFramebuffer.height;
            }
            // const invalidateAttachments: GLenum[] = [];
            let clearCount = clearColors.length;
            if (!device.extensions.WEBGL_draw_buffers) {
                clearCount = 1;
            }
            for (let j = 0; j < clearCount; ++j) {
                const colorAttachment = gpuRenderPass.colorAttachments[j];
                if (colorAttachment.format !== define_18.Format.UNKNOWN) {
                    switch (colorAttachment.loadOp) {
                        case define_18.LoadOp.LOAD: break; // GL default behavior
                        case define_18.LoadOp.CLEAR: {
                            if (cache.bs.targets[0].blendColorMask !== define_18.ColorMask.ALL) {
                                gl.colorMask(true, true, true, true);
                            }
                            const clearColor = clearColors[0];
                            gl.clearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
                            clears |= gl.COLOR_BUFFER_BIT;
                            break;
                        }
                        case define_18.LoadOp.DISCARD: {
                            // invalidate the framebuffer
                            // invalidateAttachments.push(gl.COLOR_ATTACHMENT0 + j);
                            break;
                        }
                        default:
                    }
                }
            } // if (curGPURenderPass)
            if (gpuRenderPass.depthStencilAttachment) {
                if (gpuRenderPass.depthStencilAttachment.format !== define_18.Format.UNKNOWN) {
                    switch (gpuRenderPass.depthStencilAttachment.depthLoadOp) {
                        case define_18.LoadOp.LOAD: break; // GL default behavior
                        case define_18.LoadOp.CLEAR: {
                            if (!cache.dss.depthWrite) {
                                gl.depthMask(true);
                            }
                            gl.clearDepth(clearDepth);
                            clears |= gl.DEPTH_BUFFER_BIT;
                            break;
                        }
                        case define_18.LoadOp.DISCARD: {
                            // invalidate the framebuffer
                            // invalidateAttachments.push(gl.DEPTH_ATTACHMENT);
                            break;
                        }
                        default:
                    }
                    if (define_18.FormatInfos[gpuRenderPass.depthStencilAttachment.format].hasStencil) {
                        switch (gpuRenderPass.depthStencilAttachment.stencilLoadOp) {
                            case define_18.LoadOp.LOAD: break; // GL default behavior
                            case define_18.LoadOp.CLEAR: {
                                if (!cache.dss.stencilWriteMaskFront) {
                                    gl.stencilMaskSeparate(gl.FRONT, 0xffff);
                                }
                                if (!cache.dss.stencilWriteMaskBack) {
                                    gl.stencilMaskSeparate(gl.BACK, 0xffff);
                                }
                                gl.clearStencil(clearStencil);
                                clears |= gl.STENCIL_BUFFER_BIT;
                                break;
                            }
                            case define_18.LoadOp.DISCARD: {
                                // invalidate the framebuffer
                                // invalidateAttachments.push(gl.STENCIL_ATTACHMENT);
                                break;
                            }
                            default:
                        }
                    }
                }
            } // if (gpuRenderPass.depthStencilAttachment)
            /*
            if (invalidateAttachments.length) {
                gl.invalidateFramebuffer(gl.FRAMEBUFFER, invalidateAttachments);
            }
            */
            if (clears) {
                gl.clear(clears);
            }
            // restore states
            if (clears & gl.COLOR_BUFFER_BIT) {
                const colorMask = cache.bs.targets[0].blendColorMask;
                if (colorMask !== define_18.ColorMask.ALL) {
                    const r = (colorMask & define_18.ColorMask.R) !== define_18.ColorMask.NONE;
                    const g = (colorMask & define_18.ColorMask.G) !== define_18.ColorMask.NONE;
                    const b = (colorMask & define_18.ColorMask.B) !== define_18.ColorMask.NONE;
                    const a = (colorMask & define_18.ColorMask.A) !== define_18.ColorMask.NONE;
                    gl.colorMask(r, g, b, a);
                }
            }
            if ((clears & gl.DEPTH_BUFFER_BIT)
                && !cache.dss.depthWrite) {
                gl.depthMask(false);
            }
            if (clears & gl.STENCIL_BUFFER_BIT) {
                if (!cache.dss.stencilWriteMaskFront) {
                    gl.stencilMaskSeparate(gl.FRONT, 0);
                }
                if (!cache.dss.stencilWriteMaskBack) {
                    gl.stencilMaskSeparate(gl.BACK, 0);
                }
            }
        } // if (gpuFramebuffer)
    }
    exports_27("WebGLCmdFuncBeginRenderPass", WebGLCmdFuncBeginRenderPass);
    function WebGLCmdFuncBindStates(device, gpuPipelineState, gpuInputAssembler, gpuDescriptorSets, dynamicOffsets, dynamicStates) {
        const { gl } = device;
        const cache = device.stateCache;
        const gpuShader = gpuPipelineState && gpuPipelineState.gpuShader;
        let isShaderChanged = false;
        let glWrapS;
        let glWrapT;
        let glMinFilter;
        // bind pipeline
        if (gpuPipelineState && gfxStateCache.gpuPipelineState !== gpuPipelineState) {
            gfxStateCache.gpuPipelineState = gpuPipelineState;
            gfxStateCache.glPrimitive = gpuPipelineState.glPrimitive;
            if (gpuPipelineState.gpuShader) {
                const { glProgram } = gpuPipelineState.gpuShader;
                if (cache.glProgram !== glProgram) {
                    gl.useProgram(glProgram);
                    cache.glProgram = glProgram;
                    isShaderChanged = true;
                }
            }
            // rasterizer state
            const { rs } = gpuPipelineState;
            if (rs) {
                if (cache.rs.cullMode !== rs.cullMode) {
                    switch (rs.cullMode) {
                        case define_18.CullMode.NONE: {
                            gl.disable(gl.CULL_FACE);
                            break;
                        }
                        case define_18.CullMode.FRONT: {
                            gl.enable(gl.CULL_FACE);
                            gl.cullFace(gl.FRONT);
                            break;
                        }
                        case define_18.CullMode.BACK: {
                            gl.enable(gl.CULL_FACE);
                            gl.cullFace(gl.BACK);
                            break;
                        }
                        default:
                    }
                    cache.rs.cullMode = rs.cullMode;
                }
                const isFrontFaceCCW = rs.isFrontFaceCCW;
                if (cache.rs.isFrontFaceCCW !== isFrontFaceCCW) {
                    gl.frontFace(isFrontFaceCCW ? gl.CCW : gl.CW);
                    cache.rs.isFrontFaceCCW = isFrontFaceCCW;
                }
                if ((cache.rs.depthBias !== rs.depthBias)
                    || (cache.rs.depthBiasSlop !== rs.depthBiasSlop)) {
                    gl.polygonOffset(rs.depthBias, rs.depthBiasSlop);
                    cache.rs.depthBias = rs.depthBias;
                    cache.rs.depthBiasSlop = rs.depthBiasSlop;
                }
                if (cache.rs.lineWidth !== rs.lineWidth) {
                    gl.lineWidth(rs.lineWidth);
                    cache.rs.lineWidth = rs.lineWidth;
                }
            } // rasterizater state
            // depth-stencil state
            const { dss } = gpuPipelineState;
            if (dss) {
                if (cache.dss.depthTest !== dss.depthTest) {
                    if (dss.depthTest) {
                        gl.enable(gl.DEPTH_TEST);
                    }
                    else {
                        gl.disable(gl.DEPTH_TEST);
                    }
                    cache.dss.depthTest = dss.depthTest;
                }
                if (cache.dss.depthWrite !== dss.depthWrite) {
                    gl.depthMask(dss.depthWrite);
                    cache.dss.depthWrite = dss.depthWrite;
                }
                if (cache.dss.depthFunc !== dss.depthFunc) {
                    gl.depthFunc(WebGLCmpFuncs[dss.depthFunc]);
                    cache.dss.depthFunc = dss.depthFunc;
                }
                // front
                if ((cache.dss.stencilTestFront !== dss.stencilTestFront)
                    || (cache.dss.stencilTestBack !== dss.stencilTestBack)) {
                    if (dss.stencilTestFront || dss.stencilTestBack) {
                        gl.enable(gl.STENCIL_TEST);
                    }
                    else {
                        gl.disable(gl.STENCIL_TEST);
                    }
                    cache.dss.stencilTestFront = dss.stencilTestFront;
                    cache.dss.stencilTestBack = dss.stencilTestBack;
                }
                if ((cache.dss.stencilFuncFront !== dss.stencilFuncFront)
                    || (cache.dss.stencilRefFront !== dss.stencilRefFront)
                    || (cache.dss.stencilReadMaskFront !== dss.stencilReadMaskFront)) {
                    gl.stencilFuncSeparate(gl.FRONT, WebGLCmpFuncs[dss.stencilFuncFront], dss.stencilRefFront, dss.stencilReadMaskFront);
                    cache.dss.stencilFuncFront = dss.stencilFuncFront;
                    cache.dss.stencilRefFront = dss.stencilRefFront;
                    cache.dss.stencilReadMaskFront = dss.stencilReadMaskFront;
                }
                if ((cache.dss.stencilFailOpFront !== dss.stencilFailOpFront)
                    || (cache.dss.stencilZFailOpFront !== dss.stencilZFailOpFront)
                    || (cache.dss.stencilPassOpFront !== dss.stencilPassOpFront)) {
                    gl.stencilOpSeparate(gl.FRONT, WebGLStencilOps[dss.stencilFailOpFront], WebGLStencilOps[dss.stencilZFailOpFront], WebGLStencilOps[dss.stencilPassOpFront]);
                    cache.dss.stencilFailOpFront = dss.stencilFailOpFront;
                    cache.dss.stencilZFailOpFront = dss.stencilZFailOpFront;
                    cache.dss.stencilPassOpFront = dss.stencilPassOpFront;
                }
                if (cache.dss.stencilWriteMaskFront !== dss.stencilWriteMaskFront) {
                    gl.stencilMaskSeparate(gl.FRONT, dss.stencilWriteMaskFront);
                    cache.dss.stencilWriteMaskFront = dss.stencilWriteMaskFront;
                }
                // back
                if ((cache.dss.stencilFuncBack !== dss.stencilFuncBack)
                    || (cache.dss.stencilRefBack !== dss.stencilRefBack)
                    || (cache.dss.stencilReadMaskBack !== dss.stencilReadMaskBack)) {
                    gl.stencilFuncSeparate(gl.BACK, WebGLCmpFuncs[dss.stencilFuncBack], dss.stencilRefBack, dss.stencilReadMaskBack);
                    cache.dss.stencilFuncBack = dss.stencilFuncBack;
                    cache.dss.stencilRefBack = dss.stencilRefBack;
                    cache.dss.stencilReadMaskBack = dss.stencilReadMaskBack;
                }
                if ((cache.dss.stencilFailOpBack !== dss.stencilFailOpBack)
                    || (cache.dss.stencilZFailOpBack !== dss.stencilZFailOpBack)
                    || (cache.dss.stencilPassOpBack !== dss.stencilPassOpBack)) {
                    gl.stencilOpSeparate(gl.BACK, WebGLStencilOps[dss.stencilFailOpBack], WebGLStencilOps[dss.stencilZFailOpBack], WebGLStencilOps[dss.stencilPassOpBack]);
                    cache.dss.stencilFailOpBack = dss.stencilFailOpBack;
                    cache.dss.stencilZFailOpBack = dss.stencilZFailOpBack;
                    cache.dss.stencilPassOpBack = dss.stencilPassOpBack;
                }
                if (cache.dss.stencilWriteMaskBack !== dss.stencilWriteMaskBack) {
                    gl.stencilMaskSeparate(gl.BACK, dss.stencilWriteMaskBack);
                    cache.dss.stencilWriteMaskBack = dss.stencilWriteMaskBack;
                }
            } // depth-stencil state
            // blend state
            const { bs } = gpuPipelineState;
            if (bs) {
                if (cache.bs.isA2C !== bs.isA2C) {
                    if (bs.isA2C) {
                        gl.enable(gl.SAMPLE_ALPHA_TO_COVERAGE);
                    }
                    else {
                        gl.disable(gl.SAMPLE_ALPHA_TO_COVERAGE);
                    }
                    cache.bs.isA2C = bs.isA2C;
                }
                if ((cache.bs.blendColor.x !== bs.blendColor.x)
                    || (cache.bs.blendColor.y !== bs.blendColor.y)
                    || (cache.bs.blendColor.z !== bs.blendColor.z)
                    || (cache.bs.blendColor.w !== bs.blendColor.w)) {
                    gl.blendColor(bs.blendColor.x, bs.blendColor.y, bs.blendColor.z, bs.blendColor.w);
                    cache.bs.blendColor.x = bs.blendColor.x;
                    cache.bs.blendColor.y = bs.blendColor.y;
                    cache.bs.blendColor.z = bs.blendColor.z;
                    cache.bs.blendColor.w = bs.blendColor.w;
                }
                const target0 = bs.targets[0];
                const target0Cache = cache.bs.targets[0];
                if (target0Cache.blend !== target0.blend) {
                    if (target0.blend) {
                        gl.enable(gl.BLEND);
                    }
                    else {
                        gl.disable(gl.BLEND);
                    }
                    target0Cache.blend = target0.blend;
                }
                if ((target0Cache.blendEq !== target0.blendEq)
                    || (target0Cache.blendAlphaEq !== target0.blendAlphaEq)) {
                    gl.blendEquationSeparate(WebGLBlendOps[target0.blendEq], WebGLBlendOps[target0.blendAlphaEq]);
                    target0Cache.blendEq = target0.blendEq;
                    target0Cache.blendAlphaEq = target0.blendAlphaEq;
                }
                if ((target0Cache.blendSrc !== target0.blendSrc)
                    || (target0Cache.blendDst !== target0.blendDst)
                    || (target0Cache.blendSrcAlpha !== target0.blendSrcAlpha)
                    || (target0Cache.blendDstAlpha !== target0.blendDstAlpha)) {
                    gl.blendFuncSeparate(WebGLBlendFactors[target0.blendSrc], WebGLBlendFactors[target0.blendDst], WebGLBlendFactors[target0.blendSrcAlpha], WebGLBlendFactors[target0.blendDstAlpha]);
                    target0Cache.blendSrc = target0.blendSrc;
                    target0Cache.blendDst = target0.blendDst;
                    target0Cache.blendSrcAlpha = target0.blendSrcAlpha;
                    target0Cache.blendDstAlpha = target0.blendDstAlpha;
                }
                if (target0Cache.blendColorMask !== target0.blendColorMask) {
                    gl.colorMask((target0.blendColorMask & define_18.ColorMask.R) !== define_18.ColorMask.NONE, (target0.blendColorMask & define_18.ColorMask.G) !== define_18.ColorMask.NONE, (target0.blendColorMask & define_18.ColorMask.B) !== define_18.ColorMask.NONE, (target0.blendColorMask & define_18.ColorMask.A) !== define_18.ColorMask.NONE);
                    target0Cache.blendColorMask = target0.blendColorMask;
                }
            } // blend state
        } // bind pipeline
        // bind descriptor sets
        if (gpuPipelineState && gpuPipelineState.gpuPipelineLayout && gpuShader) {
            const blockLen = gpuShader.glBlocks.length;
            const { dynamicOffsetIndices } = gpuPipelineState.gpuPipelineLayout;
            for (let j = 0; j < blockLen; j++) {
                const glBlock = gpuShader.glBlocks[j];
                const gpuDescriptorSet = gpuDescriptorSets[glBlock.set];
                const descriptorIdx = gpuDescriptorSet && gpuDescriptorSet.descriptorIndices[glBlock.binding];
                const gpuDescriptor = descriptorIdx >= 0 && gpuDescriptorSet.gpuDescriptors[descriptorIdx];
                let vf32 = null;
                let offset = 0;
                if (gpuDescriptor && gpuDescriptor.gpuBuffer) {
                    const { gpuBuffer } = gpuDescriptor;
                    const dynamicOffsetIndexSet = dynamicOffsetIndices[glBlock.set];
                    const dynamicOffsetIndex = dynamicOffsetIndexSet && dynamicOffsetIndexSet[glBlock.binding];
                    if (dynamicOffsetIndex >= 0) {
                        offset = dynamicOffsets[dynamicOffsetIndex];
                    }
                    if ('vf32' in gpuBuffer) {
                        vf32 = gpuBuffer.vf32;
                    }
                    else {
                        offset += gpuBuffer.offset;
                        vf32 = gpuBuffer.gpuBuffer.vf32;
                    }
                    offset >>= 2;
                }
                if (!vf32) {
                    debug_1.error(`Buffer binding '${glBlock.name}' at set ${glBlock.set} binding ${glBlock.binding} is not bounded`);
                    continue;
                }
                const uniformLen = glBlock.glActiveUniforms.length;
                for (let l = 0; l < uniformLen; l++) {
                    const glUniform = glBlock.glActiveUniforms[l];
                    switch (glUniform.glType) {
                        case gl.BOOL:
                        case gl.INT: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniform1iv(glUniform.glLoc, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.BOOL_VEC2:
                        case gl.INT_VEC2: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniform2iv(glUniform.glLoc, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.BOOL_VEC3:
                        case gl.INT_VEC3: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniform3iv(glUniform.glLoc, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.BOOL_VEC4:
                        case gl.INT_VEC4: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniform4iv(glUniform.glLoc, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.FLOAT: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniform1fv(glUniform.glLoc, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.FLOAT_VEC2: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniform2fv(glUniform.glLoc, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.FLOAT_VEC3: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniform3fv(glUniform.glLoc, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.FLOAT_VEC4: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniform4fv(glUniform.glLoc, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.FLOAT_MAT2: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniformMatrix2fv(glUniform.glLoc, false, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.FLOAT_MAT3: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniformMatrix3fv(glUniform.glLoc, false, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        case gl.FLOAT_MAT4: {
                            for (let u = 0; u < glUniform.array.length; ++u) {
                                const idx = glUniform.offset + offset + u;
                                if (vf32[idx] !== glUniform.array[u]) {
                                    for (let n = u, m = idx; n < glUniform.array.length; ++n, ++m) {
                                        glUniform.array[n] = vf32[m];
                                    }
                                    gl.uniformMatrix4fv(glUniform.glLoc, false, glUniform.array);
                                    break;
                                }
                            }
                            break;
                        }
                        default:
                    }
                }
                continue;
            }
            const samplerLen = gpuShader.glSamplerTextures.length;
            for (let i = 0; i < samplerLen; i++) {
                const glSampler = gpuShader.glSamplerTextures[i];
                const gpuDescriptorSet = gpuDescriptorSets[glSampler.set];
                let descriptorIndex = gpuDescriptorSet && gpuDescriptorSet.descriptorIndices[glSampler.binding];
                let gpuDescriptor = descriptorIndex >= 0 && gpuDescriptorSet.gpuDescriptors[descriptorIndex];
                const texUnitLen = glSampler.units.length;
                for (let l = 0; l < texUnitLen; l++) {
                    const texUnit = glSampler.units[l];
                    if (!gpuDescriptor || !gpuDescriptor.gpuSampler) {
                        debug_1.error(`Sampler binding '${glSampler.name}' at set ${glSampler.set} binding ${glSampler.binding} index ${l} is not bounded`);
                        continue;
                    }
                    if (gpuDescriptor.gpuTexture && gpuDescriptor.gpuTexture.size > 0) {
                        const { gpuTexture } = gpuDescriptor;
                        const glTexUnit = cache.glTexUnits[texUnit];
                        if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                            if (cache.texUnit !== texUnit) {
                                gl.activeTexture(gl.TEXTURE0 + texUnit);
                                cache.texUnit = texUnit;
                            }
                            if (gpuTexture.glTexture) {
                                gl.bindTexture(gpuTexture.glTarget, gpuTexture.glTexture);
                            }
                            else {
                                gl.bindTexture(gpuTexture.glTarget, device.nullTex2D.gpuTexture.glTexture);
                            }
                            glTexUnit.glTexture = gpuTexture.glTexture;
                        }
                        const { gpuSampler } = gpuDescriptor;
                        if (gpuTexture.isPowerOf2) {
                            glWrapS = gpuSampler.glWrapS;
                            glWrapT = gpuSampler.glWrapT;
                        }
                        else {
                            glWrapS = gl.CLAMP_TO_EDGE;
                            glWrapT = gl.CLAMP_TO_EDGE;
                        }
                        if (gpuTexture.isPowerOf2) {
                            if (gpuTexture.mipLevel <= 1
                                && (gpuSampler.glMinFilter === gl.LINEAR_MIPMAP_NEAREST
                                    || gpuSampler.glMinFilter === gl.LINEAR_MIPMAP_LINEAR)) {
                                glMinFilter = gl.LINEAR;
                            }
                            else {
                                glMinFilter = gpuSampler.glMinFilter;
                            }
                        }
                        else if (gpuSampler.glMinFilter === gl.LINEAR
                            || gpuSampler.glMinFilter === gl.LINEAR_MIPMAP_NEAREST
                            || gpuSampler.glMinFilter === gl.LINEAR_MIPMAP_LINEAR) {
                            glMinFilter = gl.LINEAR;
                        }
                        else {
                            glMinFilter = gl.NEAREST;
                        }
                        if (gpuTexture.glWrapS !== glWrapS) {
                            if (cache.texUnit !== texUnit) {
                                gl.activeTexture(gl.TEXTURE0 + texUnit);
                                cache.texUnit = texUnit;
                            }
                            gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_WRAP_S, glWrapS);
                            gpuTexture.glWrapS = glWrapS;
                        }
                        if (gpuTexture.glWrapT !== glWrapT) {
                            if (cache.texUnit !== texUnit) {
                                gl.activeTexture(gl.TEXTURE0 + texUnit);
                                cache.texUnit = texUnit;
                            }
                            gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_WRAP_T, glWrapT);
                            gpuTexture.glWrapT = glWrapT;
                        }
                        if (gpuTexture.glMinFilter !== glMinFilter) {
                            if (cache.texUnit !== texUnit) {
                                gl.activeTexture(gl.TEXTURE0 + texUnit);
                                cache.texUnit = texUnit;
                            }
                            gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_MIN_FILTER, glMinFilter);
                            gpuTexture.glMinFilter = glMinFilter;
                        }
                        if (gpuTexture.glMagFilter !== gpuSampler.glMagFilter) {
                            if (cache.texUnit !== texUnit) {
                                gl.activeTexture(gl.TEXTURE0 + texUnit);
                                cache.texUnit = texUnit;
                            }
                            gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_MAG_FILTER, gpuSampler.glMagFilter);
                            gpuTexture.glMagFilter = gpuSampler.glMagFilter;
                        }
                    }
                    gpuDescriptor = gpuDescriptorSet.gpuDescriptors[++descriptorIndex];
                }
            }
        } // bind descriptor sets
        // bind vertex/index buffer
        if (gpuInputAssembler && gpuShader
            && (isShaderChanged || gfxStateCache.gpuInputAssembler !== gpuInputAssembler)) {
            gfxStateCache.gpuInputAssembler = gpuInputAssembler;
            const ia = device.extensions.ANGLE_instanced_arrays;
            if (device.extensions.useVAO) {
                const vao = device.extensions.OES_vertex_array_object;
                // check vao
                let glVAO = gpuInputAssembler.glVAOs.get(gpuShader.glProgram);
                if (!glVAO) {
                    glVAO = vao.createVertexArrayOES();
                    gpuInputAssembler.glVAOs.set(gpuShader.glProgram, glVAO);
                    vao.bindVertexArrayOES(glVAO);
                    gl.bindBuffer(gl.ARRAY_BUFFER, null);
                    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
                    cache.glArrayBuffer = null;
                    cache.glElementArrayBuffer = null;
                    let glAttrib;
                    const inputLen = gpuShader.glInputs.length;
                    for (let j = 0; j < inputLen; j++) {
                        const glInput = gpuShader.glInputs[j];
                        glAttrib = null;
                        const attribLen = gpuInputAssembler.glAttribs.length;
                        for (let k = 0; k < attribLen; k++) {
                            const attrib = gpuInputAssembler.glAttribs[k];
                            if (attrib.name === glInput.name) {
                                glAttrib = attrib;
                                break;
                            }
                        }
                        if (glAttrib) {
                            if (cache.glArrayBuffer !== glAttrib.glBuffer) {
                                gl.bindBuffer(gl.ARRAY_BUFFER, glAttrib.glBuffer);
                                cache.glArrayBuffer = glAttrib.glBuffer;
                            }
                            for (let c = 0; c < glAttrib.componentCount; ++c) {
                                const glLoc = glInput.glLoc + c;
                                const attribOffset = glAttrib.offset + glAttrib.size * c;
                                gl.enableVertexAttribArray(glLoc);
                                cache.glCurrentAttribLocs[glLoc] = true;
                                gl.vertexAttribPointer(glLoc, glAttrib.count, glAttrib.glType, glAttrib.isNormalized, glAttrib.stride, attribOffset);
                                if (ia) {
                                    ia.vertexAttribDivisorANGLE(glLoc, glAttrib.isInstanced ? 1 : 0);
                                }
                            }
                        }
                    }
                    const gpuBuffer = gpuInputAssembler.gpuIndexBuffer;
                    if (gpuBuffer) {
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
                    }
                    vao.bindVertexArrayOES(null);
                    gl.bindBuffer(gl.ARRAY_BUFFER, null);
                    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
                    cache.glArrayBuffer = null;
                    cache.glElementArrayBuffer = null;
                }
                if (cache.glVAO !== glVAO) {
                    vao.bindVertexArrayOES(glVAO);
                    cache.glVAO = glVAO;
                }
            }
            else {
                for (let a = 0; a < device.capabilities.maxVertexAttributes; ++a) {
                    cache.glCurrentAttribLocs[a] = false;
                }
                const inputLen = gpuShader.glInputs.length;
                for (let j = 0; j < inputLen; j++) {
                    const glInput = gpuShader.glInputs[j];
                    let glAttrib = null;
                    const attribLen = gpuInputAssembler.glAttribs.length;
                    for (let k = 0; k < attribLen; k++) {
                        const attrib = gpuInputAssembler.glAttribs[k];
                        if (attrib.name === glInput.name) {
                            glAttrib = attrib;
                            break;
                        }
                    }
                    if (glAttrib) {
                        if (cache.glArrayBuffer !== glAttrib.glBuffer) {
                            gl.bindBuffer(gl.ARRAY_BUFFER, glAttrib.glBuffer);
                            cache.glArrayBuffer = glAttrib.glBuffer;
                        }
                        for (let c = 0; c < glAttrib.componentCount; ++c) {
                            const glLoc = glInput.glLoc + c;
                            const attribOffset = glAttrib.offset + glAttrib.size * c;
                            if (!cache.glEnabledAttribLocs[glLoc] && glLoc >= 0) {
                                gl.enableVertexAttribArray(glLoc);
                                cache.glEnabledAttribLocs[glLoc] = true;
                            }
                            cache.glCurrentAttribLocs[glLoc] = true;
                            gl.vertexAttribPointer(glLoc, glAttrib.count, glAttrib.glType, glAttrib.isNormalized, glAttrib.stride, attribOffset);
                            if (ia) {
                                ia.vertexAttribDivisorANGLE(glLoc, glAttrib.isInstanced ? 1 : 0);
                            }
                        }
                    }
                } // for
                const gpuBuffer = gpuInputAssembler.gpuIndexBuffer;
                if (gpuBuffer) {
                    if (cache.glElementArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
                        cache.glElementArrayBuffer = gpuBuffer.glBuffer;
                    }
                }
                for (let a = 0; a < device.capabilities.maxVertexAttributes; ++a) {
                    if (cache.glEnabledAttribLocs[a] !== cache.glCurrentAttribLocs[a]) {
                        gl.disableVertexAttribArray(a);
                        cache.glEnabledAttribLocs[a] = false;
                    }
                }
            }
        } // bind vertex/index buffer
        // update dynamic states
        if (gpuPipelineState && gpuPipelineState.dynamicStates.length) {
            const dsLen = gpuPipelineState.dynamicStates.length;
            for (let j = 0; j < dsLen; j++) {
                const dynamicState = gpuPipelineState.dynamicStates[j];
                switch (dynamicState) {
                    case define_18.DynamicStateFlagBit.LINE_WIDTH: {
                        if (cache.rs.lineWidth !== dynamicStates.lineWidth) {
                            gl.lineWidth(dynamicStates.lineWidth);
                            cache.rs.lineWidth = dynamicStates.lineWidth;
                        }
                        break;
                    }
                    case define_18.DynamicStateFlagBit.DEPTH_BIAS: {
                        if (cache.rs.depthBias !== dynamicStates.depthBiasConstant
                            || cache.rs.depthBiasSlop !== dynamicStates.depthBiasSlope) {
                            gl.polygonOffset(dynamicStates.depthBiasConstant, dynamicStates.depthBiasSlope);
                            cache.rs.depthBias = dynamicStates.depthBiasConstant;
                            cache.rs.depthBiasSlop = dynamicStates.depthBiasSlope;
                        }
                        break;
                    }
                    case define_18.DynamicStateFlagBit.BLEND_CONSTANTS: {
                        const blendConstant = dynamicStates.blendConstant;
                        if ((cache.bs.blendColor.x !== blendConstant.x)
                            || (cache.bs.blendColor.y !== blendConstant.y)
                            || (cache.bs.blendColor.z !== blendConstant.z)
                            || (cache.bs.blendColor.w !== blendConstant.w)) {
                            gl.blendColor(blendConstant.x, blendConstant.y, blendConstant.z, blendConstant.w);
                            cache.bs.blendColor.copy(blendConstant);
                        }
                        break;
                    }
                    case define_18.DynamicStateFlagBit.STENCIL_WRITE_MASK: {
                        const front = dynamicStates.stencilStatesFront;
                        const back = dynamicStates.stencilStatesBack;
                        if (cache.dss.stencilWriteMaskFront !== front.writeMask) {
                            gl.stencilMaskSeparate(gl.FRONT, front.writeMask);
                            cache.dss.stencilWriteMaskFront = front.writeMask;
                        }
                        if (cache.dss.stencilWriteMaskBack !== back.writeMask) {
                            gl.stencilMaskSeparate(gl.BACK, back.writeMask);
                            cache.dss.stencilWriteMaskBack = back.writeMask;
                        }
                        break;
                    }
                    case define_18.DynamicStateFlagBit.STENCIL_COMPARE_MASK: {
                        const front = dynamicStates.stencilStatesFront;
                        const back = dynamicStates.stencilStatesBack;
                        if (cache.dss.stencilRefFront !== front.reference
                            || cache.dss.stencilReadMaskFront !== front.compareMask) {
                            gl.stencilFuncSeparate(gl.FRONT, WebGLCmpFuncs[cache.dss.stencilFuncFront], front.reference, front.compareMask);
                            cache.dss.stencilRefFront = front.reference;
                            cache.dss.stencilReadMaskFront = front.compareMask;
                        }
                        if (cache.dss.stencilRefBack !== back.reference
                            || cache.dss.stencilReadMaskBack !== back.compareMask) {
                            gl.stencilFuncSeparate(gl.BACK, WebGLCmpFuncs[cache.dss.stencilFuncBack], back.reference, back.compareMask);
                            cache.dss.stencilRefBack = back.reference;
                            cache.dss.stencilReadMaskBack = back.compareMask;
                        }
                        break;
                    }
                    default:
                } // switch
            } // for
        } // update dynamic states
    }
    exports_27("WebGLCmdFuncBindStates", WebGLCmdFuncBindStates);
    function WebGLCmdFuncDraw(device, drawInfo) {
        const { gl } = device;
        const { ANGLE_instanced_arrays: ia, WEBGL_multi_draw: md } = device.extensions;
        const { gpuInputAssembler, glPrimitive } = gfxStateCache;
        if (gpuInputAssembler) {
            const indexBuffer = gpuInputAssembler.gpuIndexBuffer;
            if (gpuInputAssembler.gpuIndirectBuffer) {
                const indirects = gpuInputAssembler.gpuIndirectBuffer.indirects;
                if (indirects.drawByIndex) {
                    for (let j = 0; j < indirects.drawCount; j++) {
                        indirects.byteOffsets[j] = indirects.offsets[j] * indexBuffer.stride;
                    }
                    if (md) {
                        if (indirects.instancedDraw) {
                            md.multiDrawElementsInstancedWEBGL(glPrimitive, indirects.counts, 0, gpuInputAssembler.glIndexType, indirects.byteOffsets, 0, indirects.instances, 0, indirects.drawCount);
                        }
                        else {
                            md.multiDrawElementsWEBGL(glPrimitive, indirects.counts, 0, gpuInputAssembler.glIndexType, indirects.byteOffsets, 0, indirects.drawCount);
                        }
                    }
                    else {
                        for (let j = 0; j < indirects.drawCount; j++) {
                            if (indirects.instances[j] > 1 && ia) {
                                ia.drawElementsInstancedANGLE(glPrimitive, indirects.counts[j], gpuInputAssembler.glIndexType, indirects.byteOffsets[j], indirects.instances[j]);
                            }
                            else {
                                gl.drawElements(glPrimitive, indirects.counts[j], gpuInputAssembler.glIndexType, indirects.byteOffsets[j]);
                            }
                        }
                    }
                }
                else if (md) {
                    if (indirects.instancedDraw) {
                        md.multiDrawArraysInstancedWEBGL(glPrimitive, indirects.offsets, 0, indirects.counts, 0, indirects.instances, 0, indirects.drawCount);
                    }
                    else {
                        md.multiDrawArraysWEBGL(glPrimitive, indirects.offsets, 0, indirects.counts, 0, indirects.drawCount);
                    }
                }
                else {
                    for (let j = 0; j < indirects.drawCount; j++) {
                        if (indirects.instances[j] > 1 && ia) {
                            ia.drawArraysInstancedANGLE(glPrimitive, indirects.offsets[j], indirects.counts[j], indirects.instances[j]);
                        }
                        else {
                            gl.drawArrays(glPrimitive, indirects.offsets[j], indirects.counts[j]);
                        }
                    }
                }
            }
            else if (drawInfo.instanceCount > 1 && ia) {
                if (indexBuffer) {
                    if (drawInfo.indexCount > 0) {
                        const offset = drawInfo.firstIndex * indexBuffer.stride;
                        ia.drawElementsInstancedANGLE(glPrimitive, drawInfo.indexCount, gpuInputAssembler.glIndexType, offset, drawInfo.instanceCount);
                    }
                }
                else if (drawInfo.vertexCount > 0) {
                    ia.drawArraysInstancedANGLE(glPrimitive, drawInfo.firstVertex, drawInfo.vertexCount, drawInfo.instanceCount);
                }
            }
            else if (indexBuffer) {
                if (drawInfo.indexCount > 0) {
                    const offset = drawInfo.firstIndex * indexBuffer.stride;
                    gl.drawElements(glPrimitive, drawInfo.indexCount, gpuInputAssembler.glIndexType, offset);
                }
            }
            else if (drawInfo.vertexCount > 0) {
                gl.drawArrays(glPrimitive, drawInfo.firstVertex, drawInfo.vertexCount);
            }
        }
    }
    exports_27("WebGLCmdFuncDraw", WebGLCmdFuncDraw);
    function WebGLCmdFuncExecuteCmds(device, cmdPackage) {
        cmdIds.fill(0);
        for (let i = 0; i < cmdPackage.cmds.length; ++i) {
            const cmd = cmdPackage.cmds.array[i];
            const cmdId = cmdIds[cmd]++;
            switch (cmd) {
                case WebGLCmd.BEGIN_RENDER_PASS: {
                    const cmd0 = cmdPackage.beginRenderPassCmds.array[cmdId];
                    WebGLCmdFuncBeginRenderPass(device, cmd0.gpuRenderPass, cmd0.gpuFramebuffer, cmd0.renderArea, cmd0.clearColors, cmd0.clearDepth, cmd0.clearStencil);
                    break;
                }
                /*
                    case WebGLCmd.END_RENDER_PASS: {
                        // WebGL 1.0 doesn't support store operation of attachments.
                        // StoreOp.Store is the default GL behavior.
                        break;
                    }
                    */
                case WebGLCmd.BIND_STATES: {
                    const cmd2 = cmdPackage.bindStatesCmds.array[cmdId];
                    WebGLCmdFuncBindStates(device, cmd2.gpuPipelineState, cmd2.gpuInputAssembler, cmd2.gpuDescriptorSets, cmd2.dynamicOffsets, cmd2.dynamicStates);
                    break;
                }
                case WebGLCmd.DRAW: {
                    const cmd3 = cmdPackage.drawCmds.array[cmdId];
                    WebGLCmdFuncDraw(device, cmd3.drawInfo);
                    break;
                }
                case WebGLCmd.UPDATE_BUFFER: {
                    const cmd4 = cmdPackage.updateBufferCmds.array[cmdId];
                    WebGLCmdFuncUpdateBuffer(device, cmd4.gpuBuffer, cmd4.buffer, cmd4.offset, cmd4.size);
                    break;
                }
                case WebGLCmd.COPY_BUFFER_TO_TEXTURE: {
                    const cmd5 = cmdPackage.copyBufferToTextureCmds.array[cmdId];
                    WebGLCmdFuncCopyBuffersToTexture(device, cmd5.buffers, cmd5.gpuTexture, cmd5.regions);
                    break;
                }
                default:
            } // switch
        } // for
    }
    exports_27("WebGLCmdFuncExecuteCmds", WebGLCmdFuncExecuteCmds);
    function WebGLCmdFuncCopyTexImagesToTexture(device, texImages, gpuTexture, regions) {
        const { gl } = device;
        const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
        if (glTexUnit.glTexture !== gpuTexture.glTexture) {
            gl.bindTexture(gpuTexture.glTarget, gpuTexture.glTexture);
            glTexUnit.glTexture = gpuTexture.glTexture;
        }
        let n = 0;
        let f = 0;
        switch (gpuTexture.glTarget) {
            case gl.TEXTURE_2D: {
                for (let i = 0; i < regions.length; i++) {
                    const region = regions[i];
                    // console.debug('Copying image to texture 2D: ' + region.texExtent.width + ' x ' + region.texExtent.height);
                    gl.texSubImage2D(gl.TEXTURE_2D, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, gpuTexture.glFormat, gpuTexture.glType, texImages[n++]);
                }
                break;
            }
            case gl.TEXTURE_CUBE_MAP: {
                for (let i = 0; i < regions.length; i++) {
                    const region = regions[i];
                    // console.debug('Copying image to texture cube: ' + region.texExtent.width + ' x ' + region.texExtent.height);
                    const fcount = region.texSubres.baseArrayLayer + region.texSubres.layerCount;
                    for (f = region.texSubres.baseArrayLayer; f < fcount; ++f) {
                        gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, gpuTexture.glFormat, gpuTexture.glType, texImages[n++]);
                    }
                }
                break;
            }
            default: {
                console.error('Unsupported GL texture type, copy buffer to texture failed.');
            }
        }
        if ((gpuTexture.flags & define_18.TextureFlagBit.GEN_MIPMAP)
            && gpuTexture.isPowerOf2) {
            gl.generateMipmap(gpuTexture.glTarget);
        }
    }
    exports_27("WebGLCmdFuncCopyTexImagesToTexture", WebGLCmdFuncCopyTexImagesToTexture);
    function WebGLCmdFuncCopyBuffersToTexture(device, buffers, gpuTexture, regions) {
        const { gl } = device;
        const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
        if (glTexUnit.glTexture !== gpuTexture.glTexture) {
            gl.bindTexture(gpuTexture.glTarget, gpuTexture.glTexture);
            glTexUnit.glTexture = gpuTexture.glTexture;
        }
        let n = 0;
        let w = 1;
        let h = 1;
        let f = 0;
        const fmtInfo = define_18.FormatInfos[gpuTexture.format];
        const { isCompressed } = fmtInfo;
        switch (gpuTexture.glTarget) {
            case gl.TEXTURE_2D: {
                for (let i = 0; i < regions.length; i++) {
                    const region = regions[i];
                    w = region.texExtent.width;
                    h = region.texExtent.height;
                    // console.debug('Copying buffer to texture 2D: ' + w + ' x ' + h);
                    const pixels = buffers[n++];
                    if (!isCompressed) {
                        gl.texSubImage2D(gl.TEXTURE_2D, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, w, h, gpuTexture.glFormat, gpuTexture.glType, pixels);
                    }
                    else if (gpuTexture.glInternalFmt !== webgl_define_1.WebGLEXT.COMPRESSED_RGB_ETC1_WEBGL && !device.extensions.noCompressedTexSubImage2D) {
                        gl.compressedTexSubImage2D(gl.TEXTURE_2D, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, w, h, gpuTexture.glFormat, pixels);
                    }
                    else { // WEBGL_compressed_texture_etc1
                        gl.compressedTexImage2D(gl.TEXTURE_2D, region.texSubres.mipLevel, gpuTexture.glInternalFmt, w, h, 0, pixels);
                    }
                }
                break;
            }
            case gl.TEXTURE_CUBE_MAP: {
                for (let i = 0; i < regions.length; i++) {
                    const region = regions[i];
                    const fcount = region.texSubres.baseArrayLayer + region.texSubres.layerCount;
                    for (f = region.texSubres.baseArrayLayer; f < fcount; ++f) {
                        w = region.texExtent.width;
                        h = region.texExtent.height;
                        // console.debug('Copying buffer to texture cube: ' + w + ' x ' + h);
                        const pixels = buffers[n++];
                        if (!isCompressed) {
                            gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, w, h, gpuTexture.glFormat, gpuTexture.glType, pixels);
                        }
                        else if (gpuTexture.glInternalFmt !== webgl_define_1.WebGLEXT.COMPRESSED_RGB_ETC1_WEBGL && !device.extensions.noCompressedTexSubImage2D) {
                            gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, w, h, gpuTexture.glFormat, pixels);
                        }
                        else { // WEBGL_compressed_texture_etc1
                            gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, region.texSubres.mipLevel, gpuTexture.glInternalFmt, w, h, 0, pixels);
                        }
                    }
                }
                break;
            }
            default: {
                console.error('Unsupported GL texture type, copy buffer to texture failed.');
            }
        }
        if (gpuTexture.flags & define_18.TextureFlagBit.GEN_MIPMAP) {
            gl.generateMipmap(gpuTexture.glTarget);
        }
    }
    exports_27("WebGLCmdFuncCopyBuffersToTexture", WebGLCmdFuncCopyBuffersToTexture);
    function WebGLCmdFuncCopyTextureToBuffers(device, gpuTexture, buffers, regions) {
        const { gl } = device;
        const cache = device.stateCache;
        const framebuffer = gl.createFramebuffer();
        gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
        let x = 0;
        let y = 0;
        let w = 1;
        let h = 1;
        switch (gpuTexture.glTarget) {
            case gl.TEXTURE_2D: {
                for (let k = 0; k < regions.length; k++) {
                    const region = regions[k];
                    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gpuTexture.glTarget, gpuTexture.glTexture, region.texSubres.mipLevel);
                    x = region.texOffset.x;
                    y = region.texOffset.y;
                    w = region.texExtent.width;
                    h = region.texExtent.height;
                    gl.readPixels(x, y, w, h, gpuTexture.glFormat, gpuTexture.glType, buffers[k]);
                }
                break;
            }
            default: {
                console.error('Unsupported GL texture type, copy texture to buffers failed.');
            }
        }
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        cache.glFramebuffer = null;
        gl.deleteFramebuffer(framebuffer);
    }
    exports_27("WebGLCmdFuncCopyTextureToBuffers", WebGLCmdFuncCopyTextureToBuffers);
    return {
        setters: [
            function (cached_array_2_1) {
                cached_array_2 = cached_array_2_1;
            },
            function (debug_1_1) {
                debug_1 = debug_1_1;
            },
            function (webgl_define_1_1) {
                webgl_define_1 = webgl_define_1_1;
            },
            function (define_18_1) {
                define_18 = define_18_1;
            }
        ],
        execute: function () {
            WebGLCmpFuncs = [
                0x0200,
                0x0201,
                0x0202,
                0x0203,
                0x0204,
                0x0205,
                0x0206,
                0x0207, // WebGLRenderingContext.ALWAYS,
            ];
            WebGLStencilOps = [
                0x0000,
                0x1E00,
                0x1E01,
                0x1E02,
                0x1E03,
                0x150A,
                0x8507,
                0x8508, // WebGLRenderingContext.DECR_WRAP,
            ];
            WebGLBlendOps = [
                0x8006,
                0x800A,
                0x800B,
                0x8007,
                0x8008, // WebGLRenderingContext.MAX,
            ];
            WebGLBlendFactors = [
                0x0000,
                0x0001,
                0x0302,
                0x0304,
                0x0303,
                0x0305,
                0x0300,
                0x0306,
                0x0301,
                0x0307,
                0x0308,
                0x8001,
                0x8002,
                0x8003,
                0x8004, // WebGLRenderingContext.ONE_MINUS_CONSTANT_ALPHA,
            ];
            (function (WebGLCmd) {
                WebGLCmd[WebGLCmd["BEGIN_RENDER_PASS"] = 0] = "BEGIN_RENDER_PASS";
                WebGLCmd[WebGLCmd["END_RENDER_PASS"] = 1] = "END_RENDER_PASS";
                WebGLCmd[WebGLCmd["BIND_STATES"] = 2] = "BIND_STATES";
                WebGLCmd[WebGLCmd["DRAW"] = 3] = "DRAW";
                WebGLCmd[WebGLCmd["UPDATE_BUFFER"] = 4] = "UPDATE_BUFFER";
                WebGLCmd[WebGLCmd["COPY_BUFFER_TO_TEXTURE"] = 5] = "COPY_BUFFER_TO_TEXTURE";
                WebGLCmd[WebGLCmd["COUNT"] = 6] = "COUNT";
            })(WebGLCmd || (WebGLCmd = {}));
            exports_27("WebGLCmd", WebGLCmd);
            WebGLCmdObject = class WebGLCmdObject {
                constructor(type) {
                    this.refCount = 0;
                    this.cmdType = type;
                }
            };
            exports_27("WebGLCmdObject", WebGLCmdObject);
            WebGLCmdBeginRenderPass = class WebGLCmdBeginRenderPass extends WebGLCmdObject {
                constructor() {
                    super(WebGLCmd.BEGIN_RENDER_PASS);
                    this.gpuRenderPass = null;
                    this.gpuFramebuffer = null;
                    this.renderArea = new define_18.Rect();
                    this.clearFlag = define_18.ClearFlagBit.NONE;
                    this.clearColors = [];
                    this.clearDepth = 1.0;
                    this.clearStencil = 0;
                }
                clear() {
                    this.gpuFramebuffer = null;
                    this.clearColors.length = 0;
                }
            };
            exports_27("WebGLCmdBeginRenderPass", WebGLCmdBeginRenderPass);
            WebGLCmdBindStates = class WebGLCmdBindStates extends WebGLCmdObject {
                constructor() {
                    super(WebGLCmd.BIND_STATES);
                    this.gpuPipelineState = null;
                    this.gpuInputAssembler = null;
                    this.gpuDescriptorSets = [];
                    this.dynamicOffsets = [];
                    this.dynamicStates = new define_18.DynamicStates();
                }
                clear() {
                    this.gpuPipelineState = null;
                    this.gpuDescriptorSets.length = 0;
                    this.gpuInputAssembler = null;
                    this.dynamicOffsets.length = 0;
                }
            };
            exports_27("WebGLCmdBindStates", WebGLCmdBindStates);
            WebGLCmdDraw = class WebGLCmdDraw extends WebGLCmdObject {
                constructor() {
                    super(WebGLCmd.DRAW);
                    this.drawInfo = new define_18.DrawInfo();
                }
                clear() {
                }
            };
            exports_27("WebGLCmdDraw", WebGLCmdDraw);
            WebGLCmdUpdateBuffer = class WebGLCmdUpdateBuffer extends WebGLCmdObject {
                constructor() {
                    super(WebGLCmd.UPDATE_BUFFER);
                    this.gpuBuffer = null;
                    this.buffer = null;
                    this.offset = 0;
                    this.size = 0;
                }
                clear() {
                    this.gpuBuffer = null;
                    this.buffer = null;
                }
            };
            exports_27("WebGLCmdUpdateBuffer", WebGLCmdUpdateBuffer);
            WebGLCmdCopyBufferToTexture = class WebGLCmdCopyBufferToTexture extends WebGLCmdObject {
                constructor() {
                    super(WebGLCmd.COPY_BUFFER_TO_TEXTURE);
                    this.gpuTexture = null;
                    this.buffers = [];
                    this.regions = [];
                }
                clear() {
                    this.gpuTexture = null;
                    this.buffers.length = 0;
                    this.regions.length = 0;
                }
            };
            exports_27("WebGLCmdCopyBufferToTexture", WebGLCmdCopyBufferToTexture);
            WebGLCmdPackage = class WebGLCmdPackage {
                constructor() {
                    this.cmds = new cached_array_2.CachedArray(1);
                    this.beginRenderPassCmds = new cached_array_2.CachedArray(1);
                    this.bindStatesCmds = new cached_array_2.CachedArray(1);
                    this.drawCmds = new cached_array_2.CachedArray(1);
                    this.updateBufferCmds = new cached_array_2.CachedArray(1);
                    this.copyBufferToTextureCmds = new cached_array_2.CachedArray(1);
                }
                clearCmds(allocator) {
                    if (this.beginRenderPassCmds.length) {
                        allocator.beginRenderPassCmdPool.freeCmds(this.beginRenderPassCmds);
                        this.beginRenderPassCmds.clear();
                    }
                    if (this.bindStatesCmds.length) {
                        allocator.bindStatesCmdPool.freeCmds(this.bindStatesCmds);
                        this.bindStatesCmds.clear();
                    }
                    if (this.drawCmds.length) {
                        allocator.drawCmdPool.freeCmds(this.drawCmds);
                        this.drawCmds.clear();
                    }
                    if (this.updateBufferCmds.length) {
                        allocator.updateBufferCmdPool.freeCmds(this.updateBufferCmds);
                        this.updateBufferCmds.clear();
                    }
                    if (this.copyBufferToTextureCmds.length) {
                        allocator.copyBufferToTextureCmdPool.freeCmds(this.copyBufferToTextureCmds);
                        this.copyBufferToTextureCmds.clear();
                    }
                    this.cmds.clear();
                }
            };
            exports_27("WebGLCmdPackage", WebGLCmdPackage);
            gfxStateCache = {
                gpuPipelineState: null,
                gpuInputAssembler: null,
                glPrimitive: 0,
            };
            cmdIds = new Array(WebGLCmd.COUNT);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-buffer", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/base/buffer", "engine/cocos/core/gfx/webgl/webgl-commands", "engine/cocos/core/gfx/webgl/webgl-gpu-objects", "engine/cocos/core/gfx/webgl/webgl-define"], function (exports_28, context_28) {
    "use strict";
    var define_19, buffer_1, webgl_commands_2, webgl_gpu_objects_1, webgl_define_2, WebGLBuffer;
    var __moduleName = context_28 && context_28.id;
    return {
        setters: [
            function (define_19_1) {
                define_19 = define_19_1;
            },
            function (buffer_1_1) {
                buffer_1 = buffer_1_1;
            },
            function (webgl_commands_2_1) {
                webgl_commands_2 = webgl_commands_2_1;
            },
            function (webgl_gpu_objects_1_1) {
                webgl_gpu_objects_1 = webgl_gpu_objects_1_1;
            },
            function (webgl_define_2_1) {
                webgl_define_2 = webgl_define_2_1;
            }
        ],
        execute: function () {
            WebGLBuffer = class WebGLBuffer extends buffer_1.Buffer {
                constructor() {
                    super(...arguments);
                    this._gpuBuffer = null;
                    this._gpuBufferView = null;
                    this._uniformBuffer = null;
                }
                get gpuBuffer() {
                    return this._gpuBuffer;
                }
                get gpuBufferView() {
                    return this._gpuBufferView;
                }
                initialize(info) {
                    if ('buffer' in info) { // buffer view
                        this._isBufferView = true;
                        const buffer = info.buffer;
                        this._usage = buffer.usage;
                        this._memUsage = buffer.memUsage;
                        this._size = this._stride = info.range;
                        this._count = 1;
                        this._flags = buffer.flags;
                        this._gpuBufferView = {
                            gpuBuffer: buffer.gpuBuffer,
                            offset: info.offset,
                            range: info.range,
                        };
                    }
                    else { // native buffer
                        this._usage = info.usage;
                        this._memUsage = info.memUsage;
                        this._size = info.size;
                        this._stride = Math.max(info.stride || this._size, 1);
                        this._count = this._size / this._stride;
                        this._flags = info.flags;
                        if ((this._usage & define_19.BufferUsageBit.UNIFORM) && this._size > 0) {
                            this._uniformBuffer = new Uint8Array(this._size);
                        }
                        this._gpuBuffer = {
                            usage: this._usage,
                            memUsage: this._memUsage,
                            size: this._size,
                            stride: this._stride,
                            buffer: null,
                            vf32: null,
                            indirects: new webgl_gpu_objects_1.WebGLIndirectDrawInfos(),
                            glTarget: 0,
                            glBuffer: null,
                        };
                        if (this._usage & define_19.BufferUsageBit.UNIFORM) {
                            this._gpuBuffer.buffer = this._uniformBuffer;
                        }
                        webgl_commands_2.WebGLCmdFuncCreateBuffer(webgl_define_2.WebGLDeviceManager.instance, this._gpuBuffer);
                        webgl_define_2.WebGLDeviceManager.instance.memoryStatus.bufferSize += this._size;
                    }
                }
                destroy() {
                    if (this._gpuBuffer) {
                        webgl_commands_2.WebGLCmdFuncDestroyBuffer(webgl_define_2.WebGLDeviceManager.instance, this._gpuBuffer);
                        webgl_define_2.WebGLDeviceManager.instance.memoryStatus.bufferSize -= this._size;
                        this._gpuBuffer = null;
                    }
                    if (this._gpuBufferView) {
                        this._gpuBufferView = null;
                    }
                }
                resize(size) {
                    if (this._isBufferView) {
                        console.warn('cannot resize buffer views!');
                        return;
                    }
                    const oldSize = this._size;
                    if (oldSize === size) {
                        return;
                    }
                    this._size = size;
                    this._count = this._size / this._stride;
                    if (this._uniformBuffer) {
                        this._uniformBuffer = new Uint8Array(size);
                    }
                    if (this._gpuBuffer) {
                        if (this._uniformBuffer) {
                            this._gpuBuffer.buffer = this._uniformBuffer;
                        }
                        this._gpuBuffer.size = size;
                        if (size > 0) {
                            webgl_commands_2.WebGLCmdFuncResizeBuffer(webgl_define_2.WebGLDeviceManager.instance, this._gpuBuffer);
                            webgl_define_2.WebGLDeviceManager.instance.memoryStatus.bufferSize -= oldSize;
                            webgl_define_2.WebGLDeviceManager.instance.memoryStatus.bufferSize += size;
                        }
                    }
                }
                update(buffer, size) {
                    if (this._isBufferView) {
                        console.warn('cannot update through buffer views!');
                        return;
                    }
                    let buffSize;
                    if (size !== undefined) {
                        buffSize = size;
                    }
                    else if (this._usage & define_19.BufferUsageBit.INDIRECT) {
                        buffSize = 0;
                    }
                    else {
                        buffSize = buffer.byteLength;
                    }
                    webgl_commands_2.WebGLCmdFuncUpdateBuffer(webgl_define_2.WebGLDeviceManager.instance, this._gpuBuffer, buffer, 0, buffSize);
                }
            };
            exports_28("WebGLBuffer", WebGLBuffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/states/webgl-sampler", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/base/states/sampler"], function (exports_29, context_29) {
    "use strict";
    var define_20, sampler_1, WebGLWraps, WebGLSampler;
    var __moduleName = context_29 && context_29.id;
    return {
        setters: [
            function (define_20_1) {
                define_20 = define_20_1;
            },
            function (sampler_1_1) {
                sampler_1 = sampler_1_1;
            }
        ],
        execute: function () {
            WebGLWraps = [
                0x2901,
                0x8370,
                0x812F,
                0x812F, // WebGLRenderingContext.CLAMP_TO_EDGE,
            ];
            WebGLSampler = class WebGLSampler extends sampler_1.Sampler {
                constructor(info, hash) {
                    super(info, hash);
                    this._gpuSampler = null;
                    let glMinFilter = 0;
                    let glMagFilter = 0;
                    const minFilter = this._info.minFilter;
                    const magFilter = this._info.magFilter;
                    const mipFilter = this._info.mipFilter;
                    if (minFilter === define_20.Filter.LINEAR || minFilter === define_20.Filter.ANISOTROPIC) {
                        if (mipFilter === define_20.Filter.LINEAR || mipFilter === define_20.Filter.ANISOTROPIC) {
                            glMinFilter = 0x2703; // WebGLRenderingContext.LINEAR_MIPMAP_LINEAR;
                        }
                        else if (mipFilter === define_20.Filter.POINT) {
                            glMinFilter = 0x2701; // WebGLRenderingContext.LINEAR_MIPMAP_NEAREST;
                        }
                        else {
                            glMinFilter = 0x2601; // WebGLRenderingContext.LINEAR;
                        }
                    }
                    else if (mipFilter === define_20.Filter.LINEAR || mipFilter === define_20.Filter.ANISOTROPIC) {
                        glMinFilter = 0x2702; // WebGLRenderingContext.NEAREST_MIPMAP_LINEAR;
                    }
                    else if (mipFilter === define_20.Filter.POINT) {
                        glMinFilter = 0x2700; // WebGLRenderingContext.NEAREST_MIPMAP_NEAREST;
                    }
                    else {
                        glMinFilter = 0x2600; // WebGLRenderingContext.NEAREST;
                    }
                    if (magFilter === define_20.Filter.LINEAR || magFilter === define_20.Filter.ANISOTROPIC) {
                        glMagFilter = 0x2601; // WebGLRenderingContext.LINEAR;
                    }
                    else {
                        glMagFilter = 0x2600; // WebGLRenderingContext.NEAREST;
                    }
                    const glWrapS = WebGLWraps[this._info.addressU];
                    const glWrapT = WebGLWraps[this._info.addressV];
                    const glWrapR = WebGLWraps[this._info.addressW];
                    this._gpuSampler = {
                        glMinFilter,
                        glMagFilter,
                        glWrapS,
                        glWrapT,
                        glWrapR,
                    };
                }
                get gpuSampler() {
                    return this._gpuSampler;
                }
            };
            exports_29("WebGLSampler", WebGLSampler);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-texture", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/base/texture", "engine/cocos/core/gfx/webgl/webgl-commands", "engine/cocos/core/gfx/webgl/webgl-define"], function (exports_30, context_30) {
    "use strict";
    var define_21, texture_1, webgl_commands_3, webgl_define_3, WebGLTexture;
    var __moduleName = context_30 && context_30.id;
    return {
        setters: [
            function (define_21_1) {
                define_21 = define_21_1;
            },
            function (texture_1_1) {
                texture_1 = texture_1_1;
            },
            function (webgl_commands_3_1) {
                webgl_commands_3 = webgl_commands_3_1;
            },
            function (webgl_define_3_1) {
                webgl_define_3 = webgl_define_3_1;
            }
        ],
        execute: function () {
            WebGLTexture = class WebGLTexture extends texture_1.Texture {
                constructor() {
                    super(...arguments);
                    this._gpuTexture = null;
                }
                get gpuTexture() {
                    return this._gpuTexture;
                }
                initialize(info, isSwapchainTexture) {
                    if ('texture' in info) {
                        console.log('WebGL does not support texture view.');
                        return;
                    }
                    this._type = info.type;
                    this._usage = info.usage;
                    this._format = info.format;
                    this._width = info.width;
                    this._height = info.height;
                    this._depth = info.depth;
                    this._layerCount = info.layerCount;
                    this._levelCount = info.levelCount;
                    this._samples = info.samples;
                    this._flags = info.flags;
                    this._isPowerOf2 = define_21.IsPowerOf2(this._width) && define_21.IsPowerOf2(this._height);
                    this._size = define_21.FormatSurfaceSize(this._format, this.width, this.height, this.depth, this._levelCount) * this._layerCount;
                    this._gpuTexture = {
                        type: this._type,
                        format: this._format,
                        usage: this._usage,
                        width: this._width,
                        height: this._height,
                        depth: this._depth,
                        size: this._size,
                        arrayLayer: this._layerCount,
                        mipLevel: this._levelCount,
                        samples: this._samples,
                        flags: this._flags,
                        isPowerOf2: this._isPowerOf2,
                        glTarget: 0,
                        glInternalFmt: 0,
                        glFormat: 0,
                        glType: 0,
                        glUsage: 0,
                        glTexture: null,
                        glRenderbuffer: null,
                        glWrapS: 0,
                        glWrapT: 0,
                        glMinFilter: 0,
                        glMagFilter: 0,
                        isSwapchainTexture: isSwapchainTexture || false,
                    };
                    webgl_commands_3.WebGLCmdFuncCreateTexture(webgl_define_3.WebGLDeviceManager.instance, this._gpuTexture);
                    webgl_define_3.WebGLDeviceManager.instance.memoryStatus.textureSize += this._size;
                }
                destroy() {
                    if (this._gpuTexture) {
                        webgl_commands_3.WebGLCmdFuncDestroyTexture(webgl_define_3.WebGLDeviceManager.instance, this._gpuTexture);
                        webgl_define_3.WebGLDeviceManager.instance.memoryStatus.textureSize -= this._size;
                        this._gpuTexture = null;
                    }
                }
                resize(width, height) {
                    const oldSize = this._size;
                    this._width = width;
                    this._height = height;
                    this._size = define_21.FormatSurfaceSize(this._format, this.width, this.height, this.depth, this._levelCount) * this._layerCount;
                    if (this._gpuTexture) {
                        this._gpuTexture.width = width;
                        this._gpuTexture.height = height;
                        this._gpuTexture.size = this._size;
                        webgl_commands_3.WebGLCmdFuncResizeTexture(webgl_define_3.WebGLDeviceManager.instance, this._gpuTexture);
                        webgl_define_3.WebGLDeviceManager.instance.memoryStatus.textureSize -= oldSize;
                        webgl_define_3.WebGLDeviceManager.instance.memoryStatus.textureSize += this._size;
                    }
                }
                // ======================= Swapchain Specific ======================= //
                initAsSwapchainTexture(info) {
                    const texInfo = new define_21.TextureInfo();
                    texInfo.format = info.format;
                    texInfo.usage = define_21.FormatInfos[info.format].hasDepth ? define_21.TextureUsageBit.DEPTH_STENCIL_ATTACHMENT : define_21.TextureUsageBit.COLOR_ATTACHMENT;
                    texInfo.width = info.width;
                    texInfo.height = info.height;
                    this.initialize(texInfo, true);
                }
            };
            exports_30("WebGLTexture", WebGLTexture);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-descriptor-set-layout", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/base/descriptor-set-layout"], function (exports_31, context_31) {
    "use strict";
    var define_22, descriptor_set_layout_1, WebGLDescriptorSetLayout;
    var __moduleName = context_31 && context_31.id;
    return {
        setters: [
            function (define_22_1) {
                define_22 = define_22_1;
            },
            function (descriptor_set_layout_1_1) {
                descriptor_set_layout_1 = descriptor_set_layout_1_1;
            }
        ],
        execute: function () {
            WebGLDescriptorSetLayout = class WebGLDescriptorSetLayout extends descriptor_set_layout_1.DescriptorSetLayout {
                constructor() {
                    super(...arguments);
                    this._gpuDescriptorSetLayout = null;
                }
                get gpuDescriptorSetLayout() { return this._gpuDescriptorSetLayout; }
                initialize(info) {
                    Array.prototype.push.apply(this._bindings, info.bindings);
                    let descriptorCount = 0;
                    let maxBinding = -1;
                    const flattenedIndices = [];
                    for (let i = 0; i < this._bindings.length; i++) {
                        const binding = this._bindings[i];
                        flattenedIndices.push(descriptorCount);
                        descriptorCount += binding.count;
                        if (binding.binding > maxBinding)
                            maxBinding = binding.binding;
                    }
                    this._bindingIndices = Array(maxBinding + 1).fill(-1);
                    const descriptorIndices = this._descriptorIndices = Array(maxBinding + 1).fill(-1);
                    for (let i = 0; i < this._bindings.length; i++) {
                        const binding = this._bindings[i];
                        this._bindingIndices[binding.binding] = i;
                        descriptorIndices[binding.binding] = flattenedIndices[i];
                    }
                    const dynamicBindings = [];
                    for (let i = 0; i < this._bindings.length; i++) {
                        const binding = this._bindings[i];
                        if (binding.descriptorType & define_22.DESCRIPTOR_DYNAMIC_TYPE) {
                            for (let j = 0; j < binding.count; j++) {
                                dynamicBindings.push(binding.binding);
                            }
                        }
                    }
                    this._gpuDescriptorSetLayout = {
                        bindings: this._bindings,
                        dynamicBindings,
                        descriptorIndices,
                        descriptorCount,
                    };
                }
                destroy() {
                    this._bindings.length = 0;
                }
            };
            exports_31("WebGLDescriptorSetLayout", WebGLDescriptorSetLayout);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-descriptor-set", ["engine/cocos/core/gfx/base/descriptor-set", "engine/cocos/core/gfx/base/define"], function (exports_32, context_32) {
    "use strict";
    var descriptor_set_1, define_23, WebGLDescriptorSet;
    var __moduleName = context_32 && context_32.id;
    return {
        setters: [
            function (descriptor_set_1_1) {
                descriptor_set_1 = descriptor_set_1_1;
            },
            function (define_23_1) {
                define_23 = define_23_1;
            }
        ],
        execute: function () {
            WebGLDescriptorSet = class WebGLDescriptorSet extends descriptor_set_1.DescriptorSet {
                constructor() {
                    super(...arguments);
                    this._gpuDescriptorSet = null;
                }
                get gpuDescriptorSet() {
                    return this._gpuDescriptorSet;
                }
                initialize(info) {
                    this._layout = info.layout;
                    const { bindings, descriptorIndices, descriptorCount } = info.layout.gpuDescriptorSetLayout;
                    this._buffers = Array(descriptorCount).fill(null);
                    this._textures = Array(descriptorCount).fill(null);
                    this._samplers = Array(descriptorCount).fill(null);
                    const gpuDescriptors = [];
                    this._gpuDescriptorSet = { gpuDescriptors, descriptorIndices };
                    for (let i = 0; i < bindings.length; ++i) {
                        const binding = bindings[i];
                        for (let j = 0; j < binding.count; j++) {
                            gpuDescriptors.push({
                                type: binding.descriptorType,
                                gpuBuffer: null,
                                gpuTexture: null,
                                gpuSampler: null,
                            });
                        }
                    }
                }
                destroy() {
                    this._layout = null;
                    this._gpuDescriptorSet = null;
                }
                update() {
                    if (this._isDirty && this._gpuDescriptorSet) {
                        const descriptors = this._gpuDescriptorSet.gpuDescriptors;
                        for (let i = 0; i < descriptors.length; ++i) {
                            if (descriptors[i].type & define_23.DESCRIPTOR_BUFFER_TYPE) {
                                const buffer = this._buffers[i];
                                if (buffer) {
                                    descriptors[i].gpuBuffer = buffer.gpuBuffer || buffer.gpuBufferView;
                                }
                            }
                            else if (descriptors[i].type & define_23.DESCRIPTOR_SAMPLER_TYPE) {
                                if (this._textures[i]) {
                                    descriptors[i].gpuTexture = this._textures[i].gpuTexture;
                                }
                                if (this._samplers[i]) {
                                    descriptors[i].gpuSampler = this._samplers[i].gpuSampler;
                                }
                            }
                        }
                        this._isDirty = false;
                    }
                }
            };
            exports_32("WebGLDescriptorSet", WebGLDescriptorSet);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-render-pass", ["engine/cocos/core/gfx/base/render-pass"], function (exports_33, context_33) {
    "use strict";
    var render_pass_1, WebGLRenderPass;
    var __moduleName = context_33 && context_33.id;
    return {
        setters: [
            function (render_pass_1_1) {
                render_pass_1 = render_pass_1_1;
            }
        ],
        execute: function () {
            WebGLRenderPass = class WebGLRenderPass extends render_pass_1.RenderPass {
                constructor() {
                    super(...arguments);
                    this._gpuRenderPass = null;
                }
                get gpuRenderPass() {
                    return this._gpuRenderPass;
                }
                initialize(info) {
                    this._colorInfos = info.colorAttachments;
                    this._depthStencilInfo = info.depthStencilAttachment;
                    this._subpasses = info.subpasses;
                    this._gpuRenderPass = {
                        colorAttachments: this._colorInfos,
                        depthStencilAttachment: this._depthStencilInfo,
                    };
                    this._hash = this.computeHash();
                }
                destroy() {
                    this._gpuRenderPass = null;
                }
            };
            exports_33("WebGLRenderPass", WebGLRenderPass);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-framebuffer", ["engine/cocos/core/gfx/base/framebuffer", "engine/cocos/core/gfx/webgl/webgl-commands", "engine/cocos/core/gfx/webgl/webgl-define"], function (exports_34, context_34) {
    "use strict";
    var framebuffer_1, webgl_commands_4, webgl_define_4, WebGLFramebuffer;
    var __moduleName = context_34 && context_34.id;
    return {
        setters: [
            function (framebuffer_1_1) {
                framebuffer_1 = framebuffer_1_1;
            },
            function (webgl_commands_4_1) {
                webgl_commands_4 = webgl_commands_4_1;
            },
            function (webgl_define_4_1) {
                webgl_define_4 = webgl_define_4_1;
            }
        ],
        execute: function () {
            WebGLFramebuffer = class WebGLFramebuffer extends framebuffer_1.Framebuffer {
                constructor() {
                    super(...arguments);
                    this._gpuFramebuffer = null;
                }
                get gpuFramebuffer() {
                    return this._gpuFramebuffer;
                }
                initialize(info) {
                    this._renderPass = info.renderPass;
                    this._colorTextures = info.colorTextures || [];
                    this._depthStencilTexture = info.depthStencilTexture || null;
                    const gpuColorTextures = [];
                    for (let i = 0; i < info.colorTextures.length; ++i) {
                        const colorTexture = info.colorTextures[i];
                        if (colorTexture) {
                            gpuColorTextures.push(colorTexture.gpuTexture);
                        }
                    }
                    let gpuDepthStencilTexture = null;
                    if (info.depthStencilTexture) {
                        gpuDepthStencilTexture = info.depthStencilTexture.gpuTexture;
                    }
                    let width = Number.MAX_SAFE_INTEGER;
                    let height = Number.MAX_SAFE_INTEGER;
                    this._gpuFramebuffer = {
                        gpuRenderPass: info.renderPass.gpuRenderPass,
                        gpuColorTextures,
                        gpuDepthStencilTexture,
                        glFramebuffer: null,
                        isOffscreen: true,
                        get width() {
                            return this.isOffscreen ? width : this.gpuColorTextures[0].width;
                        },
                        set width(val) {
                            width = val;
                        },
                        get height() {
                            return this.isOffscreen ? height : this.gpuColorTextures[0].height;
                        },
                        set height(val) {
                            height = val;
                        },
                    };
                    webgl_commands_4.WebGLCmdFuncCreateFramebuffer(webgl_define_4.WebGLDeviceManager.instance, this._gpuFramebuffer);
                }
                destroy() {
                    if (this._gpuFramebuffer) {
                        webgl_commands_4.WebGLCmdFuncDestroyFramebuffer(webgl_define_4.WebGLDeviceManager.instance, this._gpuFramebuffer);
                        this._gpuFramebuffer = null;
                    }
                }
            };
            exports_34("WebGLFramebuffer", WebGLFramebuffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-input-assembler", ["engine/cocos/core/gfx/base/input-assembler", "engine/cocos/core/gfx/webgl/webgl-commands", "engine/cocos/core/gfx/webgl/webgl-define"], function (exports_35, context_35) {
    "use strict";
    var input_assembler_1, webgl_commands_5, webgl_define_5, WebGLInputAssembler;
    var __moduleName = context_35 && context_35.id;
    return {
        setters: [
            function (input_assembler_1_1) {
                input_assembler_1 = input_assembler_1_1;
            },
            function (webgl_commands_5_1) {
                webgl_commands_5 = webgl_commands_5_1;
            },
            function (webgl_define_5_1) {
                webgl_define_5 = webgl_define_5_1;
            }
        ],
        execute: function () {
            WebGLInputAssembler = class WebGLInputAssembler extends input_assembler_1.InputAssembler {
                constructor() {
                    super(...arguments);
                    this._gpuInputAssembler = null;
                }
                get gpuInputAssembler() {
                    return this._gpuInputAssembler;
                }
                initialize(info) {
                    if (info.vertexBuffers.length === 0) {
                        console.error('InputAssemblerInfo.vertexBuffers is null.');
                        return;
                    }
                    this._attributes = info.attributes;
                    this._attributesHash = this.computeAttributesHash();
                    this._vertexBuffers = info.vertexBuffers;
                    if (info.indexBuffer) {
                        this._indexBuffer = info.indexBuffer;
                        this._drawInfo.indexCount = this._indexBuffer.size / this._indexBuffer.stride;
                        this._drawInfo.firstIndex = 0;
                    }
                    else {
                        const vertBuff = this._vertexBuffers[0];
                        this._drawInfo.vertexCount = vertBuff.size / vertBuff.stride;
                        this._drawInfo.firstVertex = 0;
                        this._drawInfo.vertexOffset = 0;
                    }
                    this._drawInfo.instanceCount = 0;
                    this._drawInfo.firstInstance = 0;
                    this._indirectBuffer = info.indirectBuffer || null;
                    const gpuVertexBuffers = new Array(info.vertexBuffers.length);
                    for (let i = 0; i < info.vertexBuffers.length; ++i) {
                        const vb = info.vertexBuffers[i];
                        if (vb.gpuBuffer) {
                            gpuVertexBuffers[i] = vb.gpuBuffer;
                        }
                    }
                    let gpuIndexBuffer = null;
                    let glIndexType = 0;
                    if (info.indexBuffer) {
                        gpuIndexBuffer = info.indexBuffer.gpuBuffer;
                        if (gpuIndexBuffer) {
                            switch (gpuIndexBuffer.stride) {
                                case 1:
                                    glIndexType = 0x1401;
                                    break; // WebGLRenderingContext.UNSIGNED_BYTE
                                case 2:
                                    glIndexType = 0x1403;
                                    break; // WebGLRenderingContext.UNSIGNED_SHORT
                                case 4:
                                    glIndexType = 0x1405;
                                    break; // WebGLRenderingContext.UNSIGNED_INT
                                default: {
                                    console.error('Error index buffer stride.');
                                }
                            }
                        }
                    }
                    let gpuIndirectBuffer = null;
                    if (info.indirectBuffer) {
                        gpuIndirectBuffer = info.indirectBuffer.gpuBuffer;
                    }
                    this._gpuInputAssembler = {
                        attributes: info.attributes,
                        gpuVertexBuffers,
                        gpuIndexBuffer,
                        gpuIndirectBuffer,
                        glAttribs: [],
                        glIndexType,
                        glVAOs: new Map(),
                    };
                    webgl_commands_5.WebGLCmdFuncCreateInputAssember(webgl_define_5.WebGLDeviceManager.instance, this._gpuInputAssembler);
                }
                destroy() {
                    const device = webgl_define_5.WebGLDeviceManager.instance;
                    if (this._gpuInputAssembler && device.extensions.useVAO) {
                        webgl_commands_5.WebGLCmdFuncDestroyInputAssembler(device, this._gpuInputAssembler);
                    }
                    this._gpuInputAssembler = null;
                }
            };
            exports_35("WebGLInputAssembler", WebGLInputAssembler);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-shader", ["engine/cocos/core/gfx/base/shader", "engine/cocos/core/gfx/webgl/webgl-commands", "engine/cocos/core/gfx/webgl/webgl-define"], function (exports_36, context_36) {
    "use strict";
    var shader_1, webgl_commands_6, webgl_define_6, WebGLShader;
    var __moduleName = context_36 && context_36.id;
    return {
        setters: [
            function (shader_1_1) {
                shader_1 = shader_1_1;
            },
            function (webgl_commands_6_1) {
                webgl_commands_6 = webgl_commands_6_1;
            },
            function (webgl_define_6_1) {
                webgl_define_6 = webgl_define_6_1;
            }
        ],
        execute: function () {
            WebGLShader = class WebGLShader extends shader_1.Shader {
                constructor() {
                    super(...arguments);
                    this._gpuShader = null;
                }
                get gpuShader() {
                    return this._gpuShader;
                }
                initialize(info) {
                    this._name = info.name;
                    this._stages = info.stages;
                    this._attributes = info.attributes;
                    this._blocks = info.blocks;
                    this._samplers = info.samplers;
                    this._gpuShader = {
                        name: info.name,
                        blocks: info.blocks.slice(),
                        samplerTextures: info.samplerTextures.slice(),
                        subpassInputs: info.subpassInputs.slice(),
                        gpuStages: new Array(info.stages.length),
                        glProgram: null,
                        glInputs: [],
                        glUniforms: [],
                        glBlocks: [],
                        glSamplerTextures: [],
                    };
                    for (let i = 0; i < info.stages.length; ++i) {
                        const stage = info.stages[i];
                        this._gpuShader.gpuStages[i] = {
                            type: stage.stage,
                            source: stage.source,
                            glShader: null,
                        };
                    }
                    webgl_commands_6.WebGLCmdFuncCreateShader(webgl_define_6.WebGLDeviceManager.instance, this._gpuShader);
                }
                destroy() {
                    if (this._gpuShader) {
                        webgl_commands_6.WebGLCmdFuncDestroyShader(webgl_define_6.WebGLDeviceManager.instance, this._gpuShader);
                        this._gpuShader = null;
                    }
                }
            };
            exports_36("WebGLShader", WebGLShader);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-pipeline-layout", ["engine/cocos/core/gfx/base/pipeline-layout"], function (exports_37, context_37) {
    "use strict";
    var pipeline_layout_1, WebGLPipelineLayout;
    var __moduleName = context_37 && context_37.id;
    return {
        setters: [
            function (pipeline_layout_1_1) {
                pipeline_layout_1 = pipeline_layout_1_1;
            }
        ],
        execute: function () {
            WebGLPipelineLayout = class WebGLPipelineLayout extends pipeline_layout_1.PipelineLayout {
                constructor() {
                    super(...arguments);
                    this._gpuPipelineLayout = null;
                }
                get gpuPipelineLayout() { return this._gpuPipelineLayout; }
                initialize(info) {
                    Array.prototype.push.apply(this._setLayouts, info.setLayouts);
                    const dynamicOffsetIndices = [];
                    const gpuSetLayouts = [];
                    let dynamicOffsetCount = 0;
                    const dynamicOffsetOffsets = [];
                    for (let i = 0; i < this._setLayouts.length; i++) {
                        const setLayout = this._setLayouts[i];
                        const dynamicBindings = setLayout.gpuDescriptorSetLayout.dynamicBindings;
                        const indices = Array(setLayout.bindingIndices.length).fill(-1);
                        for (let j = 0; j < dynamicBindings.length; j++) {
                            const binding = dynamicBindings[j];
                            if (indices[binding] < 0)
                                indices[binding] = dynamicOffsetCount + j;
                        }
                        gpuSetLayouts.push(setLayout.gpuDescriptorSetLayout);
                        dynamicOffsetIndices.push(indices);
                        dynamicOffsetOffsets.push(dynamicOffsetCount);
                        dynamicOffsetCount += dynamicBindings.length;
                    }
                    this._gpuPipelineLayout = {
                        gpuSetLayouts,
                        dynamicOffsetIndices,
                        dynamicOffsetCount,
                        dynamicOffsetOffsets,
                    };
                }
                destroy() {
                    this._setLayouts.length = 0;
                }
            };
            exports_37("WebGLPipelineLayout", WebGLPipelineLayout);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-pipeline-state", ["engine/cocos/core/gfx/base/pipeline-state"], function (exports_38, context_38) {
    "use strict";
    var pipeline_state_1, WebGLPrimitives, WebGLPipelineState;
    var __moduleName = context_38 && context_38.id;
    return {
        setters: [
            function (pipeline_state_1_1) {
                pipeline_state_1 = pipeline_state_1_1;
            }
        ],
        execute: function () {
            WebGLPrimitives = [
                0x0000,
                0x0001,
                0x0003,
                0x0002,
                0x0000,
                0x0000,
                0x0000,
                0x0004,
                0x0005,
                0x0006,
                0x0000,
                0x0000,
                0x0000,
                0x0000, // WebGLRenderingContext.NONE,
            ];
            WebGLPipelineState = class WebGLPipelineState extends pipeline_state_1.PipelineState {
                constructor() {
                    super(...arguments);
                    this._gpuPipelineState = null;
                }
                get gpuPipelineState() {
                    return this._gpuPipelineState;
                }
                initialize(info) {
                    this._primitive = info.primitive;
                    this._shader = info.shader;
                    this._pipelineLayout = info.pipelineLayout;
                    const bs = this._bs;
                    if (info.blendState) {
                        const bsInfo = info.blendState;
                        const { targets } = bsInfo;
                        if (targets) {
                            targets.forEach((t, i) => {
                                bs.setTarget(i, t);
                            });
                        }
                        if (bsInfo.isA2C !== undefined) {
                            bs.isA2C = bsInfo.isA2C;
                        }
                        if (bsInfo.isIndepend !== undefined) {
                            bs.isIndepend = bsInfo.isIndepend;
                        }
                        if (bsInfo.blendColor !== undefined) {
                            bs.blendColor = bsInfo.blendColor;
                        }
                    }
                    Object.assign(this._rs, info.rasterizerState);
                    Object.assign(this._dss, info.depthStencilState);
                    this._is = info.inputState;
                    this._renderPass = info.renderPass;
                    this._dynamicStates = info.dynamicStates;
                    const dynamicStates = [];
                    for (let i = 0; i < 31; i++) {
                        if (this._dynamicStates & (1 << i)) {
                            dynamicStates.push(1 << i);
                        }
                    }
                    this._gpuPipelineState = {
                        glPrimitive: WebGLPrimitives[info.primitive],
                        gpuShader: info.shader.gpuShader,
                        gpuPipelineLayout: info.pipelineLayout.gpuPipelineLayout,
                        rs: info.rasterizerState,
                        dss: info.depthStencilState,
                        bs: info.blendState,
                        gpuRenderPass: info.renderPass.gpuRenderPass,
                        dynamicStates,
                    };
                }
                destroy() {
                    this._gpuPipelineState = null;
                }
            };
            exports_38("WebGLPipelineState", WebGLPipelineState);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-command-buffer", ["engine/cocos/core/gfx/base/command-buffer", "engine/cocos/core/gfx/webgl/webgl-command-allocator", "engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/webgl/webgl-commands", "engine/cocos/core/gfx/webgl/webgl-define"], function (exports_39, context_39) {
    "use strict";
    var command_buffer_1, webgl_command_allocator_1, define_24, webgl_commands_7, webgl_define_7, WebGLCommandBuffer;
    var __moduleName = context_39 && context_39.id;
    return {
        setters: [
            function (command_buffer_1_1) {
                command_buffer_1 = command_buffer_1_1;
            },
            function (webgl_command_allocator_1_1) {
                webgl_command_allocator_1 = webgl_command_allocator_1_1;
            },
            function (define_24_1) {
                define_24 = define_24_1;
            },
            function (webgl_commands_7_1) {
                webgl_commands_7 = webgl_commands_7_1;
            },
            function (webgl_define_7_1) {
                webgl_define_7 = webgl_define_7_1;
            }
        ],
        execute: function () {
            WebGLCommandBuffer = class WebGLCommandBuffer extends command_buffer_1.CommandBuffer {
                constructor() {
                    super(...arguments);
                    this.cmdPackage = new webgl_commands_7.WebGLCmdPackage();
                    this._cmdAllocator = new webgl_command_allocator_1.WebGLCommandAllocator();
                    this._isInRenderPass = false;
                    this._curGPUPipelineState = null;
                    this._curGPUInputAssembler = null;
                    this._curGPUDescriptorSets = [];
                    this._curDynamicOffsets = Array(8).fill(0);
                    this._curDynamicStates = new define_24.DynamicStates();
                    this._isStateInvalied = false;
                }
                initialize(info) {
                    this._type = info.type;
                    this._queue = info.queue;
                    const setCount = webgl_define_7.WebGLDeviceManager.instance.bindingMappingInfo.bufferOffsets.length;
                    for (let i = 0; i < setCount; i++) {
                        this._curGPUDescriptorSets.push(null);
                    }
                }
                destroy() {
                    this._cmdAllocator.clearCmds(this.cmdPackage);
                }
                begin(renderPass, subpass = 0, frameBuffer) {
                    this._cmdAllocator.clearCmds(this.cmdPackage);
                    this._curGPUPipelineState = null;
                    this._curGPUInputAssembler = null;
                    this._curGPUDescriptorSets.length = 0;
                    this._numDrawCalls = 0;
                    this._numInstances = 0;
                    this._numTris = 0;
                }
                end() {
                    if (this._isStateInvalied) {
                        this.bindStates();
                    }
                    this._isInRenderPass = false;
                }
                beginRenderPass(renderPass, framebuffer, renderArea, clearColors, clearDepth, clearStencil) {
                    const cmd = this._cmdAllocator.beginRenderPassCmdPool.alloc(webgl_commands_7.WebGLCmdBeginRenderPass);
                    cmd.gpuRenderPass = renderPass.gpuRenderPass;
                    cmd.gpuFramebuffer = framebuffer.gpuFramebuffer;
                    cmd.renderArea = renderArea;
                    cmd.clearColors.length = clearColors.length;
                    for (let i = 0; i < clearColors.length; ++i) {
                        cmd.clearColors[i] = clearColors[i];
                    }
                    cmd.clearDepth = clearDepth;
                    cmd.clearStencil = clearStencil;
                    this.cmdPackage.beginRenderPassCmds.push(cmd);
                    this.cmdPackage.cmds.push(webgl_commands_7.WebGLCmd.BEGIN_RENDER_PASS);
                    this._isInRenderPass = true;
                }
                endRenderPass() {
                    this._isInRenderPass = false;
                }
                bindPipelineState(pipelineState) {
                    const gpuPipelineState = pipelineState.gpuPipelineState;
                    if (gpuPipelineState !== this._curGPUPipelineState) {
                        this._curGPUPipelineState = gpuPipelineState;
                        this._isStateInvalied = true;
                    }
                }
                bindDescriptorSet(set, descriptorSet, dynamicOffsets) {
                    var _a;
                    const gpuDescriptorSet = descriptorSet.gpuDescriptorSet;
                    if (gpuDescriptorSet !== this._curGPUDescriptorSets[set]) {
                        this._curGPUDescriptorSets[set] = gpuDescriptorSet;
                        this._isStateInvalied = true;
                    }
                    if (dynamicOffsets) {
                        const gpuPipelineLayout = (_a = this._curGPUPipelineState) === null || _a === void 0 ? void 0 : _a.gpuPipelineLayout;
                        if (gpuPipelineLayout) {
                            const offsets = this._curDynamicOffsets;
                            const idx = gpuPipelineLayout.dynamicOffsetOffsets[set];
                            for (let i = 0; i < dynamicOffsets.length; i++)
                                offsets[idx + i] = dynamicOffsets[i];
                            this._isStateInvalied = true;
                        }
                    }
                }
                bindInputAssembler(inputAssembler) {
                    const gpuInputAssembler = inputAssembler.gpuInputAssembler;
                    this._curGPUInputAssembler = gpuInputAssembler;
                    this._isStateInvalied = true;
                }
                setViewport(viewport) {
                    const cache = this._curDynamicStates.viewport;
                    if (cache.left !== viewport.left
                        || cache.top !== viewport.top
                        || cache.width !== viewport.width
                        || cache.height !== viewport.height
                        || cache.minDepth !== viewport.minDepth
                        || cache.maxDepth !== viewport.maxDepth) {
                        cache.left = viewport.left;
                        cache.top = viewport.top;
                        cache.width = viewport.width;
                        cache.height = viewport.height;
                        cache.minDepth = viewport.minDepth;
                        cache.maxDepth = viewport.maxDepth;
                        this._isStateInvalied = true;
                    }
                }
                setScissor(scissor) {
                    const cache = this._curDynamicStates.scissor;
                    if (cache.x !== scissor.x
                        || cache.y !== scissor.y
                        || cache.width !== scissor.width
                        || cache.height !== scissor.height) {
                        cache.x = scissor.x;
                        cache.y = scissor.y;
                        cache.width = scissor.width;
                        cache.height = scissor.height;
                        this._isStateInvalied = true;
                    }
                }
                setLineWidth(lineWidth) {
                    if (this._curDynamicStates.lineWidth !== lineWidth) {
                        this._curDynamicStates.lineWidth = lineWidth;
                        this._isStateInvalied = true;
                    }
                }
                setDepthBias(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor) {
                    const cache = this._curDynamicStates;
                    if (cache.depthBiasConstant !== depthBiasConstantFactor
                        || cache.depthBiasClamp !== depthBiasClamp
                        || cache.depthBiasSlope !== depthBiasSlopeFactor) {
                        cache.depthBiasConstant = depthBiasConstantFactor;
                        cache.depthBiasClamp = depthBiasClamp;
                        cache.depthBiasSlope = depthBiasSlopeFactor;
                        this._isStateInvalied = true;
                    }
                }
                setBlendConstants(blendConstants) {
                    const cache = this._curDynamicStates.blendConstant;
                    if (cache.x !== blendConstants.x
                        || cache.y !== blendConstants.y
                        || cache.z !== blendConstants.z
                        || cache.w !== blendConstants.w) {
                        cache.copy(blendConstants);
                        this._isStateInvalied = true;
                    }
                }
                setDepthBound(minDepthBounds, maxDepthBounds) {
                    const cache = this._curDynamicStates;
                    if (cache.depthMinBounds !== minDepthBounds
                        || cache.depthMaxBounds !== maxDepthBounds) {
                        cache.depthMinBounds = minDepthBounds;
                        cache.depthMaxBounds = maxDepthBounds;
                        this._isStateInvalied = true;
                    }
                }
                setStencilWriteMask(face, writeMask) {
                    const front = this._curDynamicStates.stencilStatesFront;
                    const back = this._curDynamicStates.stencilStatesBack;
                    if (face & define_24.StencilFace.FRONT) {
                        if (front.writeMask !== writeMask) {
                            front.writeMask = writeMask;
                            this._isStateInvalied = true;
                        }
                    }
                    if (face & define_24.StencilFace.BACK) {
                        if (back.writeMask !== writeMask) {
                            back.writeMask = writeMask;
                            this._isStateInvalied = true;
                        }
                    }
                }
                setStencilCompareMask(face, reference, compareMask) {
                    const front = this._curDynamicStates.stencilStatesFront;
                    const back = this._curDynamicStates.stencilStatesBack;
                    if (face & define_24.StencilFace.FRONT) {
                        if (front.compareMask !== compareMask
                            || front.reference !== reference) {
                            front.reference = reference;
                            front.compareMask = compareMask;
                            this._isStateInvalied = true;
                        }
                    }
                    if (face & define_24.StencilFace.BACK) {
                        if (back.compareMask !== compareMask
                            || back.reference !== reference) {
                            back.reference = reference;
                            back.compareMask = compareMask;
                            this._isStateInvalied = true;
                        }
                    }
                }
                draw(infoOrAssembler) {
                    if (this._type === define_24.CommandBufferType.PRIMARY && this._isInRenderPass
                        || this._type === define_24.CommandBufferType.SECONDARY) {
                        if (this._isStateInvalied) {
                            this.bindStates();
                        }
                        const info = 'drawInfo' in infoOrAssembler ? infoOrAssembler.drawInfo : infoOrAssembler;
                        const cmd = this._cmdAllocator.drawCmdPool.alloc(webgl_commands_7.WebGLCmdDraw);
                        cmd.drawInfo.copy(info);
                        this.cmdPackage.drawCmds.push(cmd);
                        this.cmdPackage.cmds.push(webgl_commands_7.WebGLCmd.DRAW);
                        ++this._numDrawCalls;
                        this._numInstances += info.instanceCount;
                        const indexCount = info.indexCount || info.vertexCount;
                        if (this._curGPUPipelineState) {
                            const glPrimitive = this._curGPUPipelineState.glPrimitive;
                            switch (glPrimitive) {
                                case 0x0004: { // WebGLRenderingContext.TRIANGLES
                                    this._numTris += indexCount / 3 * Math.max(info.instanceCount, 1);
                                    break;
                                }
                                case 0x0005: // WebGLRenderingContext.TRIANGLE_STRIP
                                case 0x0006: { // WebGLRenderingContext.TRIANGLE_FAN
                                    this._numTris += (indexCount - 2) * Math.max(info.instanceCount, 1);
                                    break;
                                }
                                default:
                            }
                        }
                    }
                    else {
                        console.error('Command \'draw\' must be recorded inside a render pass.');
                    }
                }
                updateBuffer(buffer, data, size) {
                    if (this._type === define_24.CommandBufferType.PRIMARY && !this._isInRenderPass
                        || this._type === define_24.CommandBufferType.SECONDARY) {
                        const gpuBuffer = buffer.gpuBuffer;
                        if (gpuBuffer) {
                            const cmd = this._cmdAllocator.updateBufferCmdPool.alloc(webgl_commands_7.WebGLCmdUpdateBuffer);
                            let buffSize = 0;
                            let buff = null;
                            // TODO: Have to copy to staging buffer first to make this work for the execution is deferred.
                            // But since we are using specialized primary command buffers in WebGL backends, we leave it as is for now
                            if (buffer.usage & define_24.BufferUsageBit.INDIRECT) {
                                buff = data;
                            }
                            else {
                                if (size !== undefined) {
                                    buffSize = size;
                                }
                                else {
                                    buffSize = data.byteLength;
                                }
                                buff = data;
                            }
                            cmd.gpuBuffer = gpuBuffer;
                            cmd.buffer = buff;
                            cmd.offset = 0;
                            cmd.size = buffSize;
                            this.cmdPackage.updateBufferCmds.push(cmd);
                            this.cmdPackage.cmds.push(webgl_commands_7.WebGLCmd.UPDATE_BUFFER);
                        }
                    }
                    else {
                        console.error('Command \'updateBuffer\' must be recorded outside a render pass.');
                    }
                }
                copyBuffersToTexture(buffers, texture, regions) {
                    if (this._type === define_24.CommandBufferType.PRIMARY && !this._isInRenderPass
                        || this._type === define_24.CommandBufferType.SECONDARY) {
                        const gpuTexture = texture.gpuTexture;
                        if (gpuTexture) {
                            const cmd = this._cmdAllocator.copyBufferToTextureCmdPool.alloc(webgl_commands_7.WebGLCmdCopyBufferToTexture);
                            if (cmd) {
                                cmd.gpuTexture = gpuTexture;
                                cmd.regions = regions;
                                // TODO: Have to copy to staging buffer first to make this work for the execution is deferred.
                                // But since we are using specialized primary command buffers in WebGL backends, we leave it as is for now
                                cmd.buffers = buffers;
                                this.cmdPackage.copyBufferToTextureCmds.push(cmd);
                                this.cmdPackage.cmds.push(webgl_commands_7.WebGLCmd.COPY_BUFFER_TO_TEXTURE);
                            }
                        }
                    }
                    else {
                        console.error('Command \'copyBufferToTexture\' must be recorded outside a render pass.');
                    }
                }
                execute(cmdBuffs, count) {
                    for (let i = 0; i < count; ++i) {
                        const webGLCmdBuff = cmdBuffs[i];
                        for (let c = 0; c < webGLCmdBuff.cmdPackage.beginRenderPassCmds.length; ++c) {
                            const cmd = webGLCmdBuff.cmdPackage.beginRenderPassCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.beginRenderPassCmds.push(cmd);
                        }
                        for (let c = 0; c < webGLCmdBuff.cmdPackage.bindStatesCmds.length; ++c) {
                            const cmd = webGLCmdBuff.cmdPackage.bindStatesCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.bindStatesCmds.push(cmd);
                        }
                        for (let c = 0; c < webGLCmdBuff.cmdPackage.drawCmds.length; ++c) {
                            const cmd = webGLCmdBuff.cmdPackage.drawCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.drawCmds.push(cmd);
                        }
                        for (let c = 0; c < webGLCmdBuff.cmdPackage.updateBufferCmds.length; ++c) {
                            const cmd = webGLCmdBuff.cmdPackage.updateBufferCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.updateBufferCmds.push(cmd);
                        }
                        for (let c = 0; c < webGLCmdBuff.cmdPackage.copyBufferToTextureCmds.length; ++c) {
                            const cmd = webGLCmdBuff.cmdPackage.copyBufferToTextureCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.copyBufferToTextureCmds.push(cmd);
                        }
                        this.cmdPackage.cmds.concat(webGLCmdBuff.cmdPackage.cmds.array);
                        this._numDrawCalls += webGLCmdBuff._numDrawCalls;
                        this._numInstances += webGLCmdBuff._numInstances;
                        this._numTris += webGLCmdBuff._numTris;
                    }
                }
                pipelineBarrier(globalBarrier, textureBarriers, textures) { }
                bindStates() {
                    const bindStatesCmd = this._cmdAllocator.bindStatesCmdPool.alloc(webgl_commands_7.WebGLCmdBindStates);
                    if (bindStatesCmd) {
                        bindStatesCmd.gpuPipelineState = this._curGPUPipelineState;
                        Array.prototype.push.apply(bindStatesCmd.gpuDescriptorSets, this._curGPUDescriptorSets);
                        Array.prototype.push.apply(bindStatesCmd.dynamicOffsets, this._curDynamicOffsets);
                        bindStatesCmd.gpuInputAssembler = this._curGPUInputAssembler;
                        bindStatesCmd.dynamicStates.copy(this._curDynamicStates);
                        this.cmdPackage.bindStatesCmds.push(bindStatesCmd);
                        this.cmdPackage.cmds.push(webgl_commands_7.WebGLCmd.BIND_STATES);
                        this._isStateInvalied = false;
                    }
                }
            };
            exports_39("WebGLCommandBuffer", WebGLCommandBuffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-primary-command-buffer", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/webgl/webgl-command-buffer", "engine/cocos/core/gfx/webgl/webgl-commands", "engine/cocos/core/gfx/webgl/webgl-define"], function (exports_40, context_40) {
    "use strict";
    var define_25, webgl_command_buffer_1, webgl_commands_8, webgl_define_8, WebGLPrimaryCommandBuffer;
    var __moduleName = context_40 && context_40.id;
    return {
        setters: [
            function (define_25_1) {
                define_25 = define_25_1;
            },
            function (webgl_command_buffer_1_1) {
                webgl_command_buffer_1 = webgl_command_buffer_1_1;
            },
            function (webgl_commands_8_1) {
                webgl_commands_8 = webgl_commands_8_1;
            },
            function (webgl_define_8_1) {
                webgl_define_8 = webgl_define_8_1;
            }
        ],
        execute: function () {
            WebGLPrimaryCommandBuffer = class WebGLPrimaryCommandBuffer extends webgl_command_buffer_1.WebGLCommandBuffer {
                beginRenderPass(renderPass, framebuffer, renderArea, clearColors, clearDepth, clearStencil) {
                    webgl_commands_8.WebGLCmdFuncBeginRenderPass(webgl_define_8.WebGLDeviceManager.instance, renderPass.gpuRenderPass, framebuffer.gpuFramebuffer, renderArea, clearColors, clearDepth, clearStencil);
                    this._isInRenderPass = true;
                }
                draw(infoOrAssembler) {
                    if (this._isInRenderPass) {
                        if (this._isStateInvalied) {
                            this.bindStates();
                        }
                        const info = 'drawInfo' in infoOrAssembler ? infoOrAssembler.drawInfo : infoOrAssembler;
                        webgl_commands_8.WebGLCmdFuncDraw(webgl_define_8.WebGLDeviceManager.instance, info);
                        ++this._numDrawCalls;
                        this._numInstances += info.instanceCount;
                        const indexCount = info.indexCount || info.vertexCount;
                        if (this._curGPUPipelineState) {
                            const glPrimitive = this._curGPUPipelineState.glPrimitive;
                            switch (glPrimitive) {
                                case 0x0004: { // WebGLRenderingContext.TRIANGLES
                                    this._numTris += indexCount / 3 * Math.max(info.instanceCount, 1);
                                    break;
                                }
                                case 0x0005: // WebGLRenderingContext.TRIANGLE_STRIP
                                case 0x0006: { // WebGLRenderingContext.TRIANGLE_FAN
                                    this._numTris += (indexCount - 2) * Math.max(info.instanceCount, 1);
                                    break;
                                }
                                default:
                            }
                        }
                    }
                    else {
                        console.error('Command \'draw\' must be recorded inside a render pass.');
                    }
                }
                setViewport(viewport) {
                    const { stateCache: cache, gl } = webgl_define_8.WebGLDeviceManager.instance;
                    if (cache.viewport.left !== viewport.left
                        || cache.viewport.top !== viewport.top
                        || cache.viewport.width !== viewport.width
                        || cache.viewport.height !== viewport.height) {
                        gl.viewport(viewport.left, viewport.top, viewport.width, viewport.height);
                        cache.viewport.left = viewport.left;
                        cache.viewport.top = viewport.top;
                        cache.viewport.width = viewport.width;
                        cache.viewport.height = viewport.height;
                    }
                }
                setScissor(scissor) {
                    const { stateCache: cache, gl } = webgl_define_8.WebGLDeviceManager.instance;
                    if (cache.scissorRect.x !== scissor.x
                        || cache.scissorRect.y !== scissor.y
                        || cache.scissorRect.width !== scissor.width
                        || cache.scissorRect.height !== scissor.height) {
                        gl.scissor(scissor.x, scissor.y, scissor.width, scissor.height);
                        cache.scissorRect.x = scissor.x;
                        cache.scissorRect.y = scissor.y;
                        cache.scissorRect.width = scissor.width;
                        cache.scissorRect.height = scissor.height;
                    }
                }
                updateBuffer(buffer, data, size) {
                    if (!this._isInRenderPass) {
                        const gpuBuffer = buffer.gpuBuffer;
                        if (gpuBuffer) {
                            let buffSize;
                            if (size !== undefined) {
                                buffSize = size;
                            }
                            else if (buffer.usage & define_25.BufferUsageBit.INDIRECT) {
                                buffSize = 0;
                            }
                            else {
                                buffSize = data.byteLength;
                            }
                            webgl_commands_8.WebGLCmdFuncUpdateBuffer(webgl_define_8.WebGLDeviceManager.instance, gpuBuffer, data, 0, buffSize);
                        }
                    }
                    else {
                        console.error('Command \'updateBuffer\' must be recorded outside a render pass.');
                    }
                }
                copyBuffersToTexture(buffers, texture, regions) {
                    if (!this._isInRenderPass) {
                        const gpuTexture = texture.gpuTexture;
                        if (gpuTexture) {
                            webgl_commands_8.WebGLCmdFuncCopyBuffersToTexture(webgl_define_8.WebGLDeviceManager.instance, buffers, gpuTexture, regions);
                        }
                    }
                    else {
                        console.error('Command \'copyBufferToTexture\' must be recorded outside a render pass.');
                    }
                }
                execute(cmdBuffs, count) {
                    for (let i = 0; i < count; ++i) {
                        // actually they are secondary buffers, the cast here is only for type checking
                        const webGLCmdBuff = cmdBuffs[i];
                        webgl_commands_8.WebGLCmdFuncExecuteCmds(webgl_define_8.WebGLDeviceManager.instance, webGLCmdBuff.cmdPackage);
                        this._numDrawCalls += webGLCmdBuff._numDrawCalls;
                        this._numInstances += webGLCmdBuff._numInstances;
                        this._numTris += webGLCmdBuff._numTris;
                    }
                }
                bindStates() {
                    webgl_commands_8.WebGLCmdFuncBindStates(webgl_define_8.WebGLDeviceManager.instance, this._curGPUPipelineState, this._curGPUInputAssembler, this._curGPUDescriptorSets, this._curDynamicOffsets, this._curDynamicStates);
                    this._isStateInvalied = false;
                }
            };
            exports_40("WebGLPrimaryCommandBuffer", WebGLPrimaryCommandBuffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-queue", ["engine/cocos/core/gfx/base/queue"], function (exports_41, context_41) {
    "use strict";
    var queue_1, WebGLQueue;
    var __moduleName = context_41 && context_41.id;
    return {
        setters: [
            function (queue_1_1) {
                queue_1 = queue_1_1;
            }
        ],
        execute: function () {
            WebGLQueue = class WebGLQueue extends queue_1.Queue {
                constructor() {
                    super(...arguments);
                    this.numDrawCalls = 0;
                    this.numInstances = 0;
                    this.numTris = 0;
                }
                initialize(info) {
                    this._type = info.type;
                }
                destroy() {
                }
                submit(cmdBuffs) {
                    const len = cmdBuffs.length;
                    for (let i = 0; i < len; i++) {
                        const cmdBuff = cmdBuffs[i];
                        // WebGLCmdFuncExecuteCmds( this._device as WebGLDevice, (cmdBuff as WebGLCommandBuffer).cmdPackage); // opted out
                        this.numDrawCalls += cmdBuff.numDrawCalls;
                        this.numInstances += cmdBuff.numInstances;
                        this.numTris += cmdBuff.numTris;
                    }
                }
                clear() {
                    this.numDrawCalls = 0;
                    this.numInstances = 0;
                    this.numTris = 0;
                }
            };
            exports_41("WebGLQueue", WebGLQueue);
        }
    };
});
/*
 Copyright (c) 2008-2010 Ricardo Quesada
 Copyright (c) 2011-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2020 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/
System.register("engine/cocos/core/platform/macro", ["engine/cocos/core/global-exports"], function (exports_42, context_42) {
    "use strict";
    var global_exports_2, SUPPORT_TEXTURE_FORMATS, KEY, macro;
    var __moduleName = context_42 && context_42.id;
    return {
        setters: [
            function (global_exports_2_1) {
                global_exports_2 = global_exports_2_1;
            }
        ],
        execute: function () {
            SUPPORT_TEXTURE_FORMATS = ['.astc', '.pkm', '.pvr', '.webp', '.jpg', '.jpeg', '.bmp', '.png'];
            KEY = {
                /**
                 * @en None
                 * @zh 没有分配
                 * @readonly
                 */
                none: 0,
                // android
                /**
                 * @en The back key on mobile phone
                 * @zh 移动端返回键
                 * @readonly
                 * @deprecated since v3.3
                 */
                back: 6,
                /**
                 * @en The menu key on mobile phone
                 * @zh 移动端菜单键
                 * @readonly
                 * @deprecated since v3.3
                 */
                menu: 18,
                /**
                 * @en The backspace key
                 * @zh 退格键
                 * @readonly
                 */
                backspace: 8,
                /**
                 * @en The tab key
                 * @zh Tab 键
                 * @readonly
                 */
                tab: 9,
                /**
                 * @en The enter key
                 * @zh 回车键
                 * @readonly
                 */
                enter: 13,
                /**
                 * @en The shift key
                 * @zh Shift 键
                 * @readonly
                 * @deprecated since v3.3, please use KeyCode.SHIFT_LEFT instead.
                 */
                shift: 16,
                /**
                 * @en The ctrl key
                 * @zh Ctrl 键
                 * @readonly
                 * @deprecated since v3.3, please use KeyCode.CTRL_LEFT instead.
                 */
                ctrl: 17,
                /**
                 * @en The alt key
                 * @zh Alt 键
                 * @readonly
                 * @deprecated since v3.3, please use KeyCode.ALT_LEFT instead.
                 */
                alt: 18,
                /**
                 * @en The pause key
                 * @zh 暂停键
                 * @readonly
                 */
                pause: 19,
                /**
                 * @en The caps lock key
                 * @zh 大写锁定键
                 * @readonly
                 */
                capslock: 20,
                /**
                 * @en The esc key
                 * @zh ESC 键
                 * @readonly
                 */
                escape: 27,
                /**
                 * @en The space key
                 * @zh 空格键
                 * @readonly
                 */
                space: 32,
                /**
                 * @en The page up key
                 * @zh 向上翻页键
                 * @readonly
                 */
                pageup: 33,
                /**
                 * @en The page down key
                 * @zh 向下翻页键
                 * @readonly
                 */
                pagedown: 34,
                /**
                 * @en The end key
                 * @zh 结束键
                 * @readonly
                 */
                end: 35,
                /**
                 * @en The home key
                 * @zh 主菜单键
                 * @readonly
                 */
                home: 36,
                /**
                 * @en The left key
                 * @zh 向左箭头键
                 * @readonly
                 */
                left: 37,
                /**
                 * @en The up key
                 * @zh 向上箭头键
                 * @readonly
                 */
                up: 38,
                /**
                 * @en The right key
                 * @zh 向右箭头键
                 * @readonly
                 */
                right: 39,
                /**
                 * @en The down key
                 * @zh 向下箭头键
                 * @readonly
                 */
                down: 40,
                /**
                 * @en The select key
                 * @zh Select 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                select: 41,
                /**
                 * @en The insert key
                 * @zh 插入键
                 * @readonly
                 */
                insert: 45,
                /**
                 * @en The Delete key
                 * @zh 删除键
                 * @readonly
                 */
                Delete: 46,
                /**
                 * @en The '0' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 0 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                0: 48,
                /**
                 * @en The '1' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 1 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                1: 49,
                /**
                 * @en The '2' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 2 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                2: 50,
                /**
                 * @en The '3' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 3 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                3: 51,
                /**
                 * @en The '4' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 4 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                4: 52,
                /**
                 * @en The '5' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 5 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                5: 53,
                /**
                 * @en The '6' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 6 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                6: 54,
                /**
                 * @en The '7' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 7 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                7: 55,
                /**
                 * @en The '8' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 8 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                8: 56,
                /**
                 * @en The '9' key on the top of the alphanumeric keyboard.
                 * @zh 字母键盘上的 9 键
                 * @readonly
                 * @deprecated since v3.3
                 */
                9: 57,
                /**
                 * @en The a key
                 * @zh A 键
                 * @readonly
                 */
                a: 65,
                /**
                 * @en The b key
                 * @zh B 键
                 * @readonly
                 */
                b: 66,
                /**
                 * @en The c key
                 * @zh C 键
                 * @readonly
                 */
                c: 67,
                /**
                 * @en The d key
                 * @zh D 键
                 * @readonly
                 */
                d: 68,
                /**
                 * @en The e key
                 * @zh E 键
                 * @readonly
                 */
                e: 69,
                /**
                 * @en The f key
                 * @zh F 键
                 * @readonly
                 */
                f: 70,
                /**
                 * @en The g key
                 * @zh G 键
                 * @readonly
                 */
                g: 71,
                /**
                 * @en The h key
                 * @zh H 键
                 * @readonly
                 */
                h: 72,
                /**
                 * @en The i key
                 * @zh I 键
                 * @readonly
                 */
                i: 73,
                /**
                 * @en The j key
                 * @zh J 键
                 * @readonly
                 */
                j: 74,
                /**
                 * @en The k key
                 * @zh K 键
                 * @readonly
                 */
                k: 75,
                /**
                 * @en The l key
                 * @zh L 键
                 * @readonly
                 */
                l: 76,
                /**
                 * @en The m key
                 * @zh M 键
                 * @readonly
                 */
                m: 77,
                /**
                 * @en The n key
                 * @zh N 键
                 * @readonly
                 */
                n: 78,
                /**
                 * @en The o key
                 * @zh O 键
                 * @readonly
                 */
                o: 79,
                /**
                 * @en The p key
                 * @zh P 键
                 * @readonly
                 */
                p: 80,
                /**
                 * @en The q key
                 * @zh Q 键
                 * @readonly
                 */
                q: 81,
                /**
                 * @en The r key
                 * @zh R 键
                 * @readonly
                 */
                r: 82,
                /**
                 * @en The s key
                 * @zh S 键
                 * @readonly
                 */
                s: 83,
                /**
                 * @en The t key
                 * @zh T 键
                 * @readonly
                 */
                t: 84,
                /**
                 * @en The u key
                 * @zh U 键
                 * @readonly
                 */
                u: 85,
                /**
                 * @en The v key
                 * @zh V 键
                 * @readonly
                 */
                v: 86,
                /**
                 * @en The w key
                 * @zh W 键
                 * @readonly
                 */
                w: 87,
                /**
                 * @en The x key
                 * @zh X 键
                 * @readonly
                 */
                x: 88,
                /**
                 * @en The y key
                 * @zh Y 键
                 * @readonly
                 */
                y: 89,
                /**
                 * @en The z key
                 * @zh Z 键
                 * @readonly
                 */
                z: 90,
                /**
                 * @en The numeric keypad 0
                 * @zh 数字键盘 0
                 * @readonly
                 */
                num0: 96,
                /**
                 * @en The numeric keypad 1
                 * @zh 数字键盘 1
                 * @readonly
                 */
                num1: 97,
                /**
                 * @en The numeric keypad 2
                 * @zh 数字键盘 2
                 * @readonly
                 */
                num2: 98,
                /**
                 * @en The numeric keypad 3
                 * @zh 数字键盘 3
                 * @readonly
                 */
                num3: 99,
                /**
                 * @en The numeric keypad 4
                 * @zh 数字键盘 4
                 * @readonly
                 */
                num4: 100,
                /**
                 * @en The numeric keypad 5
                 * @zh 数字键盘 5
                 * @readonly
                 */
                num5: 101,
                /**
                 * @en The numeric keypad 6
                 * @zh 数字键盘 6
                 * @readonly
                 */
                num6: 102,
                /**
                 * @en The numeric keypad 7
                 * @zh 数字键盘 7
                 * @readonly
                 */
                num7: 103,
                /**
                 * @en The numeric keypad 8
                 * @zh 数字键盘 8
                 * @readonly
                 */
                num8: 104,
                /**
                 * @en The numeric keypad 9
                 * @zh 数字键盘 9
                 * @readonly
                 */
                num9: 105,
                /**
                 * @en The numeric keypad '*'
                 * @zh 数字键盘 *
                 * @readonly
                 * @deprecated since v3.3
                 */
                '*': 106,
                /**
                 * @en The numeric keypad '+'
                 * @zh 数字键盘 +
                 * @readonly
                 * @deprecated since v3.3
                 */
                '+': 107,
                /**
                 * @en The numeric keypad '-'
                 * @zh 数字键盘 -
                 * @readonly
                 * @deprecated since v3.3
                 */
                '-': 109,
                /**
                 * @en The numeric keypad 'delete'
                 * @zh 数字键盘删除键
                 * @readonly
                 */
                numdel: 110,
                /**
                 * @en The numeric keypad '/'
                 * @zh 数字键盘 /
                 * @readonly
                 * @deprecated since v3.3
                 */
                '/': 111,
                /**
                 * @en The F1 function key
                 * @zh F1 功能键
                 * @readonly
                 */
                f1: 112,
                /**
                 * @en The F2 function key
                 * @zh F2 功能键
                 * @readonly
                 */
                f2: 113,
                /**
                 * @en The F3 function key
                 * @zh F3 功能键
                 * @readonly
                 */
                f3: 114,
                /**
                 * @en The F4 function key
                 * @zh F4 功能键
                 * @readonly
                 */
                f4: 115,
                /**
                 * @en The F5 function key
                 * @zh F5 功能键
                 * @readonly
                 */
                f5: 116,
                /**
                 * @en The F6 function key
                 * @zh F6 功能键
                 * @readonly
                 */
                f6: 117,
                /**
                 * @en The F7 function key
                 * @zh F7 功能键
                 * @readonly
                 */
                f7: 118,
                /**
                 * @en The F8 function key
                 * @zh F8 功能键
                 * @readonly
                 */
                f8: 119,
                /**
                 * @en The F9 function key
                 * @zh F9 功能键
                 * @readonly
                 */
                f9: 120,
                /**
                 * @en The F10 function key
                 * @zh F10 功能键
                 * @readonly
                 */
                f10: 121,
                /**
                 * @en The F11 function key
                 * @zh F11 功能键
                 * @readonly
                 */
                f11: 122,
                /**
                 * @en The F12 function key
                 * @zh F12 功能键
                 * @readonly
                 */
                f12: 123,
                /**
                 * @en The numlock key
                 * @zh 数字锁定键
                 * @readonly
                 */
                numlock: 144,
                /**
                 * @en The scroll lock key
                 * @zh 滚动锁定键
                 * @readonly
                 */
                scrolllock: 145,
                /**
                 * @en The ';' key.
                 * @zh 分号键
                 * @readonly
                 * @deprecated since v3.3
                 */
                ';': 186,
                /**
                 * @en The ';' key.
                 * @zh 分号键
                 * @readonly
                 */
                semicolon: 186,
                /**
                 * @en The '=' key.
                 * @zh 等于号键
                 * @readonly
                 */
                equal: 187,
                /**
                 * @en The '=' key.
                 * @zh 等于号键
                 * @readonly
                 * @deprecated since v3.3
                 */
                '=': 187,
                /**
                 * @en The ',' key.
                 * @zh 逗号键
                 * @readonly
                 * @deprecated since v3.3
                 */
                ',': 188,
                /**
                 * @en The ',' key.
                 * @zh 逗号键
                 * @readonly
                 */
                comma: 188,
                /**
                 * @en The dash '-' key.
                 * @zh 中划线键
                 * @readonly
                 */
                dash: 189,
                /**
                 * @en The '.' key.
                 * @zh 句号键
                 * @readonly
                 * @deprecated since v3.3
                 */
                '.': 190,
                /**
                 * @en The '.' key
                 * @zh 句号键
                 * @readonly
                 */
                period: 190,
                /**
                 * @en The forward slash key
                 * @zh 正斜杠键
                 * @readonly
                 */
                forwardslash: 191,
                /**
                 * @en The grave key
                 * @zh 按键 `
                 * @readonly
                 */
                grave: 192,
                /**
                 * @en The '[' key
                 * @zh 按键 [
                 * @readonly
                 * @deprecated since v3.3
                 */
                '[': 219,
                /**
                 * @en The '[' key
                 * @zh 按键 [
                 * @readonly
                 */
                openbracket: 219,
                /**
                 * @en The '\' key
                 * @zh 反斜杠键
                 * @readonly
                 */
                backslash: 220,
                /**
                 * @en The ']' key
                 * @zh 按键 ]
                 * @readonly
                 * @deprecated since v3.3
                 */
                ']': 221,
                /**
                 * @en The ']' key
                 * @zh 按键 ]
                 * @readonly
                 */
                closebracket: 221,
                /**
                 * @en The quote key
                 * @zh 单引号键
                 * @readonly
                 */
                quote: 222,
                // gamepad controll
                /**
                 * @en The dpad left key
                 * @zh 导航键 向左
                 * @readonly
                 * @deprecated since v3.3
                 */
                dpadLeft: 1000,
                /**
                 * @en The dpad right key
                 * @zh 导航键 向右
                 * @readonly
                 * @deprecated since v3.3
                 */
                dpadRight: 1001,
                /**
                 * @en The dpad up key
                 * @zh 导航键 向上
                 * @readonly
                 * @deprecated since v3.3
                 */
                dpadUp: 1003,
                /**
                 * @en The dpad down key
                 * @zh 导航键 向下
                 * @readonly
                 * @deprecated since v3.3
                 */
                dpadDown: 1004,
                /**
                 * @en The dpad center key
                 * @zh 导航键 确定键
                 * @readonly
                 * @deprecated since v3.3
                 */
                dpadCenter: 1005,
            };
            /**
             * @en
             * Predefined constants
             * @zh
             * 预定义常量。
             */
            macro = {
                /**
                 * @en
                 * The image format supported by the engine defaults, and the supported formats may differ in different build platforms and device types.
                 * Currently all platform and device support ['.webp', '.jpg', '.jpeg', '.bmp', '.png'], ios mobile platform
                 * @zh
                 * 引擎默认支持的图片格式，支持的格式可能在不同的构建平台和设备类型上有所差别。
                 * 目前所有平台和设备支持的格式有 ['.webp', '.jpg', '.jpeg', '.bmp', '.png']. The iOS mobile platform also supports the PVR format。
                 */
                SUPPORT_TEXTURE_FORMATS,
                /**
                 * @en Key map for keyboard event
                 * @zh 键盘事件的按键值。
                 * @example {@link cocos/core/platform/CCCommon/KEY.js}
                 * @deprecated since v3.3 please use KeyCode instead
                 */
                KEY,
                /**
                 * PI / 180
                 */
                RAD: Math.PI / 180,
                /**
                 * One degree
                 */
                DEG: 180 / Math.PI,
                /**
                 * A maximum value of number
                 */
                REPEAT_FOREVER: (Number.MAX_VALUE - 1),
                /**
                 * A minimal float value
                 */
                FLT_EPSILON: 0.0000001192092896,
                // Possible device orientations
                /**
                 * @en Oriented vertically
                 * @zh 竖屏朝向
                 */
                ORIENTATION_PORTRAIT: 1,
                /**
                 * @en Oriented horizontally
                 * @zh 横屏朝向
                 */
                ORIENTATION_LANDSCAPE: 2,
                /**
                 * @en Oriented automatically
                 * @zh 自动适配朝向
                 */
                ORIENTATION_AUTO: 3,
                /**
                 * <p>
                 *   If enabled, the texture coordinates will be calculated by using this formula: <br/>
                 *      - texCoord.left = (rect.x*2+1) / (texture.wide*2);                  <br/>
                 *      - texCoord.right = texCoord.left + (rect.width*2-2)/(texture.wide*2); <br/>
                 *                                                                                 <br/>
                 *  The same for bottom and top.                                                   <br/>
                 *                                                                                 <br/>
                 *  This formula prevents artifacts by using 99% of the texture.                   <br/>
                 *  The "correct" way to prevent artifacts is by expand the texture's border with the same color by 1 pixel<br/>
                 *                                                                                  <br/>
                 *  Affected component:                                                                 <br/>
                 *      - TMXLayer                                                       <br/>
                 *                                                                                  <br/>
                 *  Enabled by default. To disabled set it to 0. <br/>
                 *  To modify it, in Web engine please refer to CCMacro.js, in JSB please refer to CCConfig.h
                 * </p>
                 * Currently not useful in 3D engine
                 */
                // FIX_ARTIFACTS_BY_STRECHING_TEXEL_TMX: true,
                /**
                 * @en
                 * Whether or not enabled tiled map auto culling. If you set the TiledMap skew or rotation,
                 * then need to manually disable this, otherwise, the rendering will be wrong.
                 * Currently not useful in 3D engine
                 * @zh
                 * 是否开启瓦片地图的自动裁减功能。瓦片地图如果设置了 skew, rotation 的话，需要手动关闭，否则渲染会出错。
                 * 在 3D 引擎中暂时无效。
                 * @default true
                 */
                ENABLE_TILEDMAP_CULLING: true,
                /**
                 * @en
                 * The timeout to determine whether a touch is no longer active and should be removed.
                 * The reason to add this timeout is due to an issue in X5 browser core,
                 * when X5 is presented in wechat on Android, if a touch is glissed from the bottom up, and leave the page area,
                 * no touch cancel event is triggered, and the touch will be considered active forever.
                 * After multiple times of this action, our maximum touches number will be reached and all new touches will be ignored.
                 * So this new mechanism can remove the touch that should be inactive if it's not updated during the last 5000 milliseconds.
                 * Though it might remove a real touch if it's just not moving for the last 5 seconds which is not easy with the sensibility of mobile touch screen.
                 * You can modify this value to have a better behavior if you find it's not enough.
                 * @zh
                 * 用于甄别一个触点对象是否已经失效并且可以被移除的延时时长
                 * 添加这个时长的原因是 X5 内核在微信浏览器中出现的一个 bug。
                 * 在这个环境下，如果用户将一个触点从底向上移出页面区域，将不会触发任何 touch cancel 或 touch end 事件，而这个触点会被永远当作停留在页面上的有效触点。
                 * 重复这样操作几次之后，屏幕上的触点数量将达到我们的事件系统所支持的最高触点数量，之后所有的触摸事件都将被忽略。
                 * 所以这个新的机制可以在触点在一定时间内没有任何更新的情况下视为失效触点并从事件系统中移除。
                 * 当然，这也可能移除一个真实的触点，如果用户的触点真的在一定时间段内完全没有移动（这在当前手机屏幕的灵敏度下会很难）。
                 * 你可以修改这个值来获得你需要的效果，默认值是 5000 毫秒。
                 * @default 5000
                 */
                TOUCH_TIMEOUT: 5000,
                /**
                 * @en
                 * Boolean that indicates if the canvas contains an alpha channel, default sets to false for better performance.
                 * Though if you want to make your canvas background transparent and show other dom elements at the background,
                 * you can set it to true before {{game.init}}.
                 * Web only.
                 * @zh
                 * 用于设置 Canvas 背景是否支持 alpha 通道，默认为 false，这样可以有更高的性能表现。
                 * 如果你希望 Canvas 背景是透明的，并显示背后的其他 DOM 元素，你可以在 {{game.init}} 之前将这个值设为 true。
                 * 仅支持 Web
                 * @default false
                 */
                ENABLE_TRANSPARENT_CANVAS: false,
                /**
                 * @en
                 * Boolean that indicates if the GL context is created with `antialias` option turned on, default value is false.
                 * Set it to true could make your game graphics slightly smoother, like texture hard edges when rotated.
                 * Whether to use this really depend on your game design and targeted platform,
                 * device with retina display usually have good detail on graphics with or without this option,
                 * you probably don't want antialias if your game style is pixel art based.
                 * Also, it could have great performance impact with some browser / device using software MSAA.
                 * You can set it to true before {{game.init}}.
                 * Only affect OpenGL ES and WebGL backend
                 * @zh
                 * 用于设置在创建 GL Context 时是否开启抗锯齿选项，默认值是 false。
                 * 将这个选项设置为 true 会让你的游戏画面稍稍平滑一些，比如旋转硬边贴图时的锯齿。是否开启这个选项很大程度上取决于你的游戏和面向的平台。
                 * 在大多数拥有 retina 级别屏幕的设备上用户往往无法区分这个选项带来的变化；如果你的游戏选择像素艺术风格，你也多半不会想开启这个选项。
                 * 同时，在少部分使用软件级别抗锯齿算法的设备或浏览器上，这个选项会对性能产生比较大的影响。
                 * 你可以在 {{game.init}} 之前设置这个值，否则它不会生效。
                 * 仅影响 WebGL 后端
                 * @default true
                 */
                ENABLE_WEBGL_ANTIALIAS: true,
                /**
                 * @en
                 * Used to set fxaa post-processing anti-aliasing, the default value is false.
                 * @zh
                 * 用于开启fxaa后处理抗锯齿, 默认值为false。
                 * @default false
                 */
                ENABLE_ANTIALIAS_FXAA: false,
                /**
                 * @en
                 * Used to set bloom, the default value is false.
                 * @zh
                 * 用于开启 bloom, 默认值为false。
                 * @default false
                 */
                ENABLE_BLOOM: false,
                /**
                 * @en
                 * Whether to clear the original image cache after uploaded a texture to GPU.
                 * If cleared, [Dynamic Atlas](https://docs.cocos.com/creator/manual/en/advanced-topics/dynamic-atlas.html) will not be supported.
                 * Normally you don't need to enable this option on the web platform, because Image object doesn't consume too much memory.
                 * But on Wechat Game platform, the current version cache decoded data in Image object, which has high memory usage.
                 * So we enabled this option by default on Wechat, so that we can release Image cache immediately after uploaded to GPU.
                 * Currently not useful in 3D engine
                 * @zh
                 * 是否在将贴图上传至 GPU 之后删除原始图片缓存，删除之后图片将无法进行 [动态合图](https://docs.cocos.com/creator/manual/zh/advanced-topics/dynamic-atlas.html)。
                 * 在 Web 平台，你通常不需要开启这个选项，因为在 Web 平台 Image 对象所占用的内存很小。
                 * 但是在微信小游戏平台的当前版本，Image 对象会缓存解码后的图片数据，它所占用的内存空间很大。
                 * 所以我们在微信平台默认开启了这个选项，这样我们就可以在上传 GL 贴图之后立即释放 Image 对象的内存，避免过高的内存占用。
                 * 在 3D 引擎中暂时无效。
                 * @default false
                 */
                CLEANUP_IMAGE_CACHE: false,
                /**
                  * @en
                  * Whether to enable multi-touch.
                  * @zh
                  * 是否开启多点触摸
                  * @default true
                  */
                ENABLE_MULTI_TOUCH: true,
                /**
                 * @en
                 * The maximum size of the canvas pool used by Label, please adjust according to the number of label component in the same scene of the project
                 * @zh
                 * Label 使用的 canvas pool 的最大大小，请根据项目同场景的 label 数量进行调整
                 * @default 20
                 */
                MAX_LABEL_CANVAS_POOL_SIZE: 20,
            };
            exports_42("macro", macro);
            global_exports_2.legacyCC.macro = macro;
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-state-cache", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/base/pipeline-state"], function (exports_43, context_43) {
    "use strict";
    var define_26, pipeline_state_2, WebGLStateCache;
    var __moduleName = context_43 && context_43.id;
    return {
        setters: [
            function (define_26_1) {
                define_26 = define_26_1;
            },
            function (pipeline_state_2_1) {
                pipeline_state_2 = pipeline_state_2_1;
            }
        ],
        execute: function () {
            WebGLStateCache = class WebGLStateCache {
                constructor() {
                    this.glArrayBuffer = null;
                    this.glElementArrayBuffer = null;
                    this.glVAO = null;
                    this.texUnit = 0;
                    this.glTexUnits = [];
                    this.glRenderbuffer = null;
                    this.glFramebuffer = null;
                    this.viewport = new define_26.Viewport();
                    this.scissorRect = new define_26.Rect(0, 0, 0, 0);
                    this.rs = new pipeline_state_2.RasterizerState();
                    this.dss = new pipeline_state_2.DepthStencilState();
                    this.bs = new pipeline_state_2.BlendState();
                    this.glProgram = null;
                    this.glEnabledAttribLocs = [];
                    this.glCurrentAttribLocs = [];
                    this.texUnitCacheMap = {};
                }
                initialize(texUnit, vertexAttributes) {
                    for (let i = 0; i < texUnit; ++i)
                        this.glTexUnits.push({ glTexture: null });
                    this.glEnabledAttribLocs.length = vertexAttributes;
                    this.glEnabledAttribLocs.fill(false);
                    this.glCurrentAttribLocs.length = vertexAttributes;
                    this.glCurrentAttribLocs.fill(false);
                }
            };
            exports_43("WebGLStateCache", WebGLStateCache);
        }
    };
});
System.register("engine/pal/system-info/enum-type/browser-type", [], function (exports_44, context_44) {
    "use strict";
    var BrowserType;
    var __moduleName = context_44 && context_44.id;
    return {
        setters: [],
        execute: function () {
            (function (BrowserType) {
                /**
                 * @en Browser Type - Unknown
                 * @zh 浏览器类型 - 未知
                 */
                BrowserType["UNKNOWN"] = "unknown";
                /**
                 * @en Browser Type - WeChat inner browser
                 * @zh 浏览器类型 - 微信内置浏览器
                 */
                BrowserType["WECHAT"] = "wechat";
                /**
                 * @en Browser Type - Android Browser
                 * @zh 浏览器类型 - 安卓浏览器
                 */
                BrowserType["ANDROID"] = "androidbrowser";
                /**
                 * @en Browser Type - Internet Explorer
                 * @zh 浏览器类型 - 微软 IE
                 */
                BrowserType["IE"] = "ie";
                /**
                 * @en Browser Type - Microsoft Edge
                 * @zh 浏览器类型 - 微软 Edge
                 */
                BrowserType["EDGE"] = "edge";
                /**
                 * @en Browser Type - QQ Browser
                 * @zh 浏览器类型 - QQ 浏览器
                 */
                BrowserType["QQ"] = "qqbrowser";
                /**
                 * @en Browser Type - Mobile QQ Browser
                 * @zh 浏览器类型 - 手机 QQ 浏览器
                 */
                BrowserType["MOBILE_QQ"] = "mqqbrowser";
                /**
                 * @en Browser Type - UC Browser
                 * @zh 浏览器类型 - UC 浏览器
                 */
                BrowserType["UC"] = "ucbrowser";
                /**
                 * @en Browser Type - Third party integrated UC browser
                 * @zh 浏览器类型 - 第三方应用中集成的 UC 浏览器
                 */
                BrowserType["UCBS"] = "ucbs";
                /**
                 * @en Browser Type - 360 Browser
                 * @zh 浏览器类型 - 360 浏览器
                 */
                BrowserType["BROWSER_360"] = "360browser";
                /**
                 * @en Browser Type - Baidu Box App
                 * @zh 浏览器类型 - Baidu Box App
                 */
                BrowserType["BAIDU_APP"] = "baiduboxapp";
                /**
                 * @en Browser Type - Baidu Browser
                 * @zh 浏览器类型 - 百度浏览器
                 */
                BrowserType["BAIDU"] = "baidubrowser";
                /**
                 * @en Browser Type - Maxthon Browser
                 * @zh 浏览器类型 - 傲游浏览器
                 */
                BrowserType["MAXTHON"] = "maxthon";
                /**
                 * @en Browser Type - Opera Browser
                 * @zh 浏览器类型 - Opera 浏览器
                 */
                BrowserType["OPERA"] = "opera";
                /**
                 * @en Browser Type - Oupeng Browser
                 * @zh 浏览器类型 - 欧朋浏览器
                 */
                BrowserType["OUPENG"] = "oupeng";
                /**
                 * @en Browser Type - MI UI Browser
                 * @zh 浏览器类型 - MIUI 内置浏览器
                 */
                BrowserType["MIUI"] = "miuibrowser";
                /**
                 * @en Browser Type - Firefox Browser
                 * @zh 浏览器类型 - Firefox 浏览器
                 */
                BrowserType["FIREFOX"] = "firefox";
                /**
                 * @en Browser Type - Safari Browser
                 * @zh 浏览器类型 - Safari 浏览器
                 */
                BrowserType["SAFARI"] = "safari";
                /**
                 * @en Browser Type - Chrome Browser
                 * @zh 浏览器类型 - Chrome 浏览器
                 */
                BrowserType["CHROME"] = "chrome";
                /**
                 * @en Browser Type - Cheetah Browser
                 * @zh 浏览器类型 - 猎豹浏览器
                 */
                BrowserType["LIEBAO"] = "liebao";
                /**
                 * @en Browser Type - QZone Inner Browser
                 * @zh 浏览器类型 - QZone 内置浏览器
                 */
                BrowserType["QZONE"] = "qzone";
                /**
                 * @en Browser Type - Sogou Browser
                 * @zh 浏览器类型 - 搜狗浏览器
                 */
                BrowserType["SOUGOU"] = "sogou";
                /**
                 * @en Browser Type - Huawei Browser
                 * @zh 浏览器类型 - 华为浏览器
                 */
                BrowserType["HUAWEI"] = "huawei";
            })(BrowserType || (BrowserType = {}));
            exports_44("BrowserType", BrowserType);
        }
    };
});
System.register("engine/pal/system-info/enum-type/operating-system", [], function (exports_45, context_45) {
    "use strict";
    var OS;
    var __moduleName = context_45 && context_45.id;
    return {
        setters: [],
        execute: function () {
            (function (OS) {
                /**
                 * @en Operating System - Unknown
                 * @zh 操作系统 - 未知
                 */
                OS["UNKNOWN"] = "Unknown";
                /**
                 * @en Operating System - iOS
                 * @zh 操作系统 - iOS
                 */
                OS["IOS"] = "iOS";
                /**
                 * @en Operating System - Android
                 * @zh 操作系统 - 安卓
                 */
                OS["ANDROID"] = "Android";
                /**
                 * @en Operating System - Windows
                 * @zh 操作系统 - Windows
                 */
                OS["WINDOWS"] = "Windows";
                /**
                 * @en Operating System - Linux
                 * @zh 操作系统 - Linux
                 */
                OS["LINUX"] = "Linux";
                /**
                 * @en Operating System - Mac OS X
                 * @zh 操作系统 - Mac OS X
                 */
                OS["OSX"] = "OS X";
                /**
                 * @en Operating System - Huawei Open Harmony OS
                 * @zh 操作系统 - 鸿蒙
                 */
                OS["OHOS"] = "OHOS";
            })(OS || (OS = {}));
            exports_45("OS", OS);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-swapchain", ["internal:constants", "engine/cocos/core/platform/debug", "engine/cocos/core/platform/macro", "engine/cocos/core/gfx/webgl/webgl-command-allocator", "engine/cocos/core/gfx/webgl/webgl-state-cache", "engine/cocos/core/gfx/webgl/webgl-texture", "engine/cocos/core/gfx/base/define", "engine/pal/system-info/enum-type/browser-type", "engine/pal/system-info/enum-type/operating-system", "engine/cocos/core/gfx/base/swapchain", "engine/cocos/core/gfx/webgl/webgl-define", "engine/cocos/core/global-exports"], function (exports_46, context_46) {
    "use strict";
    var internal_constants_3, debug_2, macro_1, webgl_command_allocator_2, webgl_state_cache_1, webgl_texture_1, define_27, browser_type_1, operating_system_1, swapchain_1, webgl_define_9, global_exports_3, eventWebGLContextLost, WebGLSwapchain;
    var __moduleName = context_46 && context_46.id;
    function initStates(gl) {
        gl.activeTexture(gl.TEXTURE0);
        gl.pixelStorei(gl.PACK_ALIGNMENT, 1);
        gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, false);
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        // rasterizer state
        gl.enable(gl.SCISSOR_TEST);
        gl.enable(gl.CULL_FACE);
        gl.cullFace(gl.BACK);
        gl.frontFace(gl.CCW);
        gl.disable(gl.POLYGON_OFFSET_FILL);
        gl.polygonOffset(0.0, 0.0);
        // depth stencil state
        gl.enable(gl.DEPTH_TEST);
        gl.depthMask(true);
        gl.depthFunc(gl.LESS);
        gl.depthRange(0.0, 1.0);
        gl.stencilFuncSeparate(gl.FRONT, gl.ALWAYS, 1, 0xffff);
        gl.stencilOpSeparate(gl.FRONT, gl.KEEP, gl.KEEP, gl.KEEP);
        gl.stencilMaskSeparate(gl.FRONT, 0xffff);
        gl.stencilFuncSeparate(gl.BACK, gl.ALWAYS, 1, 0xffff);
        gl.stencilOpSeparate(gl.BACK, gl.KEEP, gl.KEEP, gl.KEEP);
        gl.stencilMaskSeparate(gl.BACK, 0xffff);
        gl.disable(gl.STENCIL_TEST);
        // blend state
        gl.disable(gl.SAMPLE_ALPHA_TO_COVERAGE);
        gl.disable(gl.BLEND);
        gl.blendEquationSeparate(gl.FUNC_ADD, gl.FUNC_ADD);
        gl.blendFuncSeparate(gl.ONE, gl.ZERO, gl.ONE, gl.ZERO);
        gl.colorMask(true, true, true, true);
        gl.blendColor(0.0, 0.0, 0.0, 0.0);
    }
    function getExtension(gl, ext) {
        const prefixes = ['', 'WEBKIT_', 'MOZ_'];
        for (let i = 0; i < prefixes.length; ++i) {
            const _ext = gl.getExtension(prefixes[i] + ext);
            if (_ext) {
                // eslint-disable-next-line @typescript-eslint/no-unsafe-return
                return _ext;
            }
        }
        return null;
    }
    function getExtensions(gl) {
        const res = {
            EXT_texture_filter_anisotropic: getExtension(gl, 'EXT_texture_filter_anisotropic'),
            EXT_blend_minmax: getExtension(gl, 'EXT_blend_minmax'),
            EXT_frag_depth: getExtension(gl, 'EXT_frag_depth'),
            EXT_shader_texture_lod: getExtension(gl, 'EXT_shader_texture_lod'),
            EXT_sRGB: getExtension(gl, 'EXT_sRGB'),
            OES_vertex_array_object: getExtension(gl, 'OES_vertex_array_object'),
            EXT_color_buffer_half_float: getExtension(gl, 'EXT_color_buffer_half_float'),
            WEBGL_multi_draw: getExtension(gl, 'WEBGL_multi_draw'),
            WEBGL_color_buffer_float: getExtension(gl, 'WEBGL_color_buffer_float'),
            WEBGL_compressed_texture_etc1: getExtension(gl, 'WEBGL_compressed_texture_etc1'),
            WEBGL_compressed_texture_etc: getExtension(gl, 'WEBGL_compressed_texture_etc'),
            WEBGL_compressed_texture_pvrtc: getExtension(gl, 'WEBGL_compressed_texture_pvrtc'),
            WEBGL_compressed_texture_s3tc: getExtension(gl, 'WEBGL_compressed_texture_s3tc'),
            WEBGL_compressed_texture_s3tc_srgb: getExtension(gl, 'WEBGL_compressed_texture_s3tc_srgb'),
            WEBGL_debug_shaders: getExtension(gl, 'WEBGL_debug_shaders'),
            WEBGL_draw_buffers: getExtension(gl, 'WEBGL_draw_buffers'),
            WEBGL_lose_context: getExtension(gl, 'WEBGL_lose_context'),
            WEBGL_depth_texture: getExtension(gl, 'WEBGL_depth_texture'),
            OES_texture_half_float: getExtension(gl, 'OES_texture_half_float'),
            OES_texture_half_float_linear: getExtension(gl, 'OES_texture_half_float_linear'),
            OES_texture_float: getExtension(gl, 'OES_texture_float'),
            OES_texture_float_linear: getExtension(gl, 'OES_texture_float_linear'),
            OES_standard_derivatives: getExtension(gl, 'OES_standard_derivatives'),
            OES_element_index_uint: getExtension(gl, 'OES_element_index_uint'),
            ANGLE_instanced_arrays: getExtension(gl, 'ANGLE_instanced_arrays'),
            WEBGL_debug_renderer_info: getExtension(gl, 'WEBGL_debug_renderer_info'),
            WEBGL_compressed_texture_astc: null,
            destroyShadersImmediately: true,
            noCompressedTexSubImage2D: false,
            isLocationActive: (glLoc) => !!glLoc,
            useVAO: false,
        };
        // platform-specific extension hacks
        // eslint-disable-next-line no-lone-blocks
        {
            // iOS 14 browsers crash on getExtension('WEBGL_compressed_texture_astc')
            if (global_exports_3.legacyCC.sys.os !== operating_system_1.OS.IOS || global_exports_3.legacyCC.sys.osMainVersion !== 14 || !global_exports_3.legacyCC.sys.isBrowser) {
                res.WEBGL_compressed_texture_astc = getExtension(gl, 'WEBGL_compressed_texture_astc');
            }
            // UC browser instancing implementation doesn't work
            if (global_exports_3.legacyCC.sys.browserType === browser_type_1.BrowserType.UC) {
                res.ANGLE_instanced_arrays = null;
            }
            // bytedance ios depth texture implementation doesn't work
            if (internal_constants_3.BYTEDANCE && global_exports_3.legacyCC.sys.os === operating_system_1.OS.IOS) {
                res.WEBGL_depth_texture = null;
            }
            if (internal_constants_3.RUNTIME_BASED) {
                // VAO implementations doesn't work well on some runtime platforms
                if (internal_constants_3.LINKSURE || internal_constants_3.QTT || internal_constants_3.COCOSPLAY || internal_constants_3.HUAWEI) {
                    res.OES_vertex_array_object = null;
                }
            }
            // some earlier version of iOS and android wechat implement gl.detachShader incorrectly
            if ((global_exports_3.legacyCC.sys.os === operating_system_1.OS.IOS && global_exports_3.legacyCC.sys.osMainVersion <= 10)
                || (internal_constants_3.WECHAT && global_exports_3.legacyCC.sys.os === operating_system_1.OS.ANDROID)) {
                res.destroyShadersImmediately = false;
            }
            // compressedTexSubImage2D has always been problematic because the
            // parameters differs slightly from GLES, and many platforms get it wrong
            if (internal_constants_3.WECHAT) {
                res.noCompressedTexSubImage2D = true;
            }
            // getUniformLocation too [eyerolling]
            if (internal_constants_3.WECHAT) {
                // wEcHaT just returns { id: -1 } for inactive names
                res.isLocationActive = (glLoc) => !!glLoc && glLoc.id !== -1;
            }
            if (internal_constants_3.ALIPAY) {
                // aLiPaY just returns the location number directly on actual devices, and WebGLUniformLocation objects in simulators
                res.isLocationActive = (glLoc) => !!glLoc && glLoc !== -1 || glLoc === 0;
            }
        }
        if (res.OES_vertex_array_object) {
            res.useVAO = true;
        }
        return res;
    }
    exports_46("getExtensions", getExtensions);
    return {
        setters: [
            function (internal_constants_3_1) {
                internal_constants_3 = internal_constants_3_1;
            },
            function (debug_2_1) {
                debug_2 = debug_2_1;
            },
            function (macro_1_1) {
                macro_1 = macro_1_1;
            },
            function (webgl_command_allocator_2_1) {
                webgl_command_allocator_2 = webgl_command_allocator_2_1;
            },
            function (webgl_state_cache_1_1) {
                webgl_state_cache_1 = webgl_state_cache_1_1;
            },
            function (webgl_texture_1_1) {
                webgl_texture_1 = webgl_texture_1_1;
            },
            function (define_27_1) {
                define_27 = define_27_1;
            },
            function (browser_type_1_1) {
                browser_type_1 = browser_type_1_1;
            },
            function (operating_system_1_1) {
                operating_system_1 = operating_system_1_1;
            },
            function (swapchain_1_1) {
                swapchain_1 = swapchain_1_1;
            },
            function (webgl_define_9_1) {
                webgl_define_9 = webgl_define_9_1;
            },
            function (global_exports_3_1) {
                global_exports_3 = global_exports_3_1;
            }
        ],
        execute: function () {
            eventWebGLContextLost = 'webglcontextlost';
            WebGLSwapchain = class WebGLSwapchain extends swapchain_1.Swapchain {
                constructor() {
                    super(...arguments);
                    this.stateCache = new webgl_state_cache_1.WebGLStateCache();
                    this.cmdAllocator = new webgl_command_allocator_2.WebGLCommandAllocator();
                    this.nullTex2D = null;
                    this.nullTexCube = null;
                    this._canvas = null;
                    this._webGLRC = null;
                    this._webGLContextLostHandler = null;
                    this._extensions = null;
                }
                get gl() {
                    return this._webGLRC;
                }
                get extensions() {
                    return this._extensions;
                }
                initialize(info) {
                    this._canvas = info.windowHandle;
                    try {
                        const webGLCtxAttribs = {
                            alpha: macro_1.macro.ENABLE_TRANSPARENT_CANVAS,
                            antialias: internal_constants_3.EDITOR || macro_1.macro.ENABLE_WEBGL_ANTIALIAS,
                            depth: true,
                            stencil: true,
                            premultipliedAlpha: false,
                            preserveDrawingBuffer: false,
                            powerPreference: 'default',
                            failIfMajorPerformanceCaveat: false,
                        };
                        this._webGLRC = this._canvas.getContext('webgl', webGLCtxAttribs);
                    }
                    catch (err) {
                        return;
                    }
                    if (!this._webGLRC) {
                        return;
                    }
                    this._webGLContextLostHandler = this._onWebGLContextLost.bind(this);
                    this._canvas.addEventListener(eventWebGLContextLost, this._onWebGLContextLost);
                    const gl = this.gl;
                    this.stateCache.initialize(webgl_define_9.WebGLDeviceManager.instance.capabilities.maxTextureUnits, webgl_define_9.WebGLDeviceManager.instance.capabilities.maxVertexAttributes);
                    this._extensions = getExtensions(gl);
                    // init states
                    initStates(gl);
                    const colorFmt = define_27.Format.RGBA8;
                    let depthStencilFmt = define_27.Format.DEPTH_STENCIL;
                    let depthBits = gl.getParameter(gl.DEPTH_BITS);
                    const stencilBits = gl.getParameter(gl.STENCIL_BITS);
                    if (internal_constants_3.ALIPAY) {
                        depthBits = 24;
                    }
                    if (depthBits && stencilBits)
                        depthStencilFmt = define_27.Format.DEPTH_STENCIL;
                    else if (depthBits)
                        depthStencilFmt = define_27.Format.DEPTH;
                    this._colorTexture = new webgl_texture_1.WebGLTexture();
                    // @ts-expect-error(2445) private initializer
                    this._colorTexture.initAsSwapchainTexture({
                        swapchain: this,
                        format: colorFmt,
                        width: info.width,
                        height: info.height,
                    });
                    this._depthStencilTexture = new webgl_texture_1.WebGLTexture();
                    // @ts-expect-error(2445) private initializer
                    this._depthStencilTexture.initAsSwapchainTexture({
                        swapchain: this,
                        format: depthStencilFmt,
                        width: info.width,
                        height: info.height,
                    });
                    // create default null texture
                    this.nullTex2D = webgl_define_9.WebGLDeviceManager.instance.createTexture(new define_27.TextureInfo(define_27.TextureType.TEX2D, define_27.TextureUsageBit.SAMPLED, define_27.Format.RGBA8, 2, 2, define_27.TextureFlagBit.GEN_MIPMAP));
                    this.nullTexCube = webgl_define_9.WebGLDeviceManager.instance.createTexture(new define_27.TextureInfo(define_27.TextureType.CUBE, define_27.TextureUsageBit.SAMPLED, define_27.Format.RGBA8, 2, 2, define_27.TextureFlagBit.GEN_MIPMAP, 6));
                    const nullTexRegion = new define_27.BufferTextureCopy();
                    nullTexRegion.texExtent.width = 2;
                    nullTexRegion.texExtent.height = 2;
                    const nullTexBuff = new Uint8Array(this.nullTex2D.size);
                    nullTexBuff.fill(0);
                    webgl_define_9.WebGLDeviceManager.instance.copyBuffersToTexture([nullTexBuff], this.nullTex2D, [nullTexRegion]);
                    nullTexRegion.texSubres.layerCount = 6;
                    webgl_define_9.WebGLDeviceManager.instance.copyBuffersToTexture([nullTexBuff, nullTexBuff, nullTexBuff, nullTexBuff, nullTexBuff, nullTexBuff], this.nullTexCube, [nullTexRegion]);
                }
                destroy() {
                    if (this._canvas && this._webGLContextLostHandler) {
                        this._canvas.removeEventListener(eventWebGLContextLost, this._webGLContextLostHandler);
                        this._webGLContextLostHandler = null;
                    }
                    if (this.nullTex2D) {
                        this.nullTex2D.destroy();
                        this.nullTex2D = null;
                    }
                    if (this.nullTexCube) {
                        this.nullTexCube.destroy();
                        this.nullTexCube = null;
                    }
                    this._extensions = null;
                    this._webGLRC = null;
                    this._canvas = null;
                }
                resize(width, height, surfaceTransform) {
                    if (this._colorTexture.width !== width || this._colorTexture.height !== height) {
                        debug_2.debug(`Resizing swapchain: ${width}x${height}`);
                        this._canvas.width = width;
                        this._canvas.height = height;
                        this._colorTexture.resize(width, height);
                        this._depthStencilTexture.resize(width, height);
                    }
                }
                _onWebGLContextLost(event) {
                    debug_2.warnID(11000);
                    debug_2.warn(event);
                    // 2020.9.3: `preventDefault` is not available on some platforms
                    // event.preventDefault();
                }
            };
            exports_46("WebGLSwapchain", WebGLSwapchain);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl/webgl-device", ["engine/cocos/core/gfx/base/device", "engine/cocos/core/gfx/base/states/sampler", "engine/cocos/core/gfx/webgl/webgl-descriptor-set", "engine/cocos/core/gfx/webgl/webgl-buffer", "engine/cocos/core/gfx/webgl/webgl-command-buffer", "engine/cocos/core/gfx/webgl/webgl-framebuffer", "engine/cocos/core/gfx/webgl/webgl-input-assembler", "engine/cocos/core/gfx/webgl/webgl-descriptor-set-layout", "engine/cocos/core/gfx/webgl/webgl-pipeline-layout", "engine/cocos/core/gfx/webgl/webgl-pipeline-state", "engine/cocos/core/gfx/webgl/webgl-primary-command-buffer", "engine/cocos/core/gfx/webgl/webgl-queue", "engine/cocos/core/gfx/webgl/webgl-render-pass", "engine/cocos/core/gfx/webgl/states/webgl-sampler", "engine/cocos/core/gfx/webgl/webgl-shader", "engine/cocos/core/gfx/webgl/webgl-swapchain", "engine/cocos/core/gfx/webgl/webgl-texture", "engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/webgl/webgl-commands", "engine/cocos/core/gfx/base/states/global-barrier", "engine/cocos/core/gfx/base/states/texture-barrier", "engine/cocos/core/platform/debug", "engine/cocos/core/gfx/webgl/webgl-define"], function (exports_47, context_47) {
    "use strict";
    var device_1, sampler_2, webgl_descriptor_set_1, webgl_buffer_1, webgl_command_buffer_2, webgl_framebuffer_1, webgl_input_assembler_1, webgl_descriptor_set_layout_1, webgl_pipeline_layout_1, webgl_pipeline_state_1, webgl_primary_command_buffer_1, webgl_queue_1, webgl_render_pass_1, webgl_sampler_1, webgl_shader_1, webgl_swapchain_1, webgl_texture_2, define_28, webgl_commands_9, global_barrier_1, texture_barrier_1, debug_3, webgl_define_10, WebGLDevice;
    var __moduleName = context_47 && context_47.id;
    return {
        setters: [
            function (device_1_1) {
                device_1 = device_1_1;
            },
            function (sampler_2_1) {
                sampler_2 = sampler_2_1;
            },
            function (webgl_descriptor_set_1_1) {
                webgl_descriptor_set_1 = webgl_descriptor_set_1_1;
            },
            function (webgl_buffer_1_1) {
                webgl_buffer_1 = webgl_buffer_1_1;
            },
            function (webgl_command_buffer_2_1) {
                webgl_command_buffer_2 = webgl_command_buffer_2_1;
            },
            function (webgl_framebuffer_1_1) {
                webgl_framebuffer_1 = webgl_framebuffer_1_1;
            },
            function (webgl_input_assembler_1_1) {
                webgl_input_assembler_1 = webgl_input_assembler_1_1;
            },
            function (webgl_descriptor_set_layout_1_1) {
                webgl_descriptor_set_layout_1 = webgl_descriptor_set_layout_1_1;
            },
            function (webgl_pipeline_layout_1_1) {
                webgl_pipeline_layout_1 = webgl_pipeline_layout_1_1;
            },
            function (webgl_pipeline_state_1_1) {
                webgl_pipeline_state_1 = webgl_pipeline_state_1_1;
            },
            function (webgl_primary_command_buffer_1_1) {
                webgl_primary_command_buffer_1 = webgl_primary_command_buffer_1_1;
            },
            function (webgl_queue_1_1) {
                webgl_queue_1 = webgl_queue_1_1;
            },
            function (webgl_render_pass_1_1) {
                webgl_render_pass_1 = webgl_render_pass_1_1;
            },
            function (webgl_sampler_1_1) {
                webgl_sampler_1 = webgl_sampler_1_1;
            },
            function (webgl_shader_1_1) {
                webgl_shader_1 = webgl_shader_1_1;
            },
            function (webgl_swapchain_1_1) {
                webgl_swapchain_1 = webgl_swapchain_1_1;
            },
            function (webgl_texture_2_1) {
                webgl_texture_2 = webgl_texture_2_1;
            },
            function (define_28_1) {
                define_28 = define_28_1;
            },
            function (webgl_commands_9_1) {
                webgl_commands_9 = webgl_commands_9_1;
            },
            function (global_barrier_1_1) {
                global_barrier_1 = global_barrier_1_1;
            },
            function (texture_barrier_1_1) {
                texture_barrier_1 = texture_barrier_1_1;
            },
            function (debug_3_1) {
                debug_3 = debug_3_1;
            },
            function (webgl_define_10_1) {
                webgl_define_10 = webgl_define_10_1;
            }
        ],
        execute: function () {
            WebGLDevice = class WebGLDevice extends device_1.Device {
                constructor() {
                    super(...arguments);
                    this._swapchain = null;
                }
                get gl() {
                    return this._swapchain.gl;
                }
                get extensions() {
                    return this._swapchain.extensions;
                }
                get stateCache() {
                    return this._swapchain.stateCache;
                }
                get nullTex2D() {
                    return this._swapchain.nullTex2D;
                }
                get nullTexCube() {
                    return this._swapchain.nullTexCube;
                }
                initialize(info) {
                    webgl_define_10.WebGLDeviceManager.setInstance(this);
                    this._gfxAPI = define_28.API.WEBGL;
                    this._bindingMappingInfo = info.bindingMappingInfo;
                    if (!this._bindingMappingInfo.bufferOffsets.length)
                        this._bindingMappingInfo.bufferOffsets.push(0);
                    if (!this._bindingMappingInfo.samplerOffsets.length)
                        this._bindingMappingInfo.samplerOffsets.push(0);
                    let gl = null;
                    try {
                        gl = document.createElement('canvas').getContext('webgl');
                    }
                    catch (err) {
                        console.error(err);
                    }
                    if (!gl) {
                        console.error('This device does not support WebGL.');
                        return false;
                    }
                    // create queue
                    this._queue = this.createQueue(new define_28.QueueInfo(define_28.QueueType.GRAPHICS));
                    this._cmdBuff = this.createCommandBuffer(new define_28.CommandBufferInfo(this._queue));
                    this._caps.maxVertexAttributes = gl.getParameter(gl.MAX_VERTEX_ATTRIBS);
                    this._caps.maxVertexUniformVectors = gl.getParameter(gl.MAX_VERTEX_UNIFORM_VECTORS);
                    this._caps.maxFragmentUniformVectors = gl.getParameter(gl.MAX_FRAGMENT_UNIFORM_VECTORS);
                    this._caps.maxTextureUnits = gl.getParameter(gl.MAX_TEXTURE_IMAGE_UNITS);
                    this._caps.maxVertexTextureUnits = gl.getParameter(gl.MAX_VERTEX_TEXTURE_IMAGE_UNITS);
                    this._caps.maxTextureSize = gl.getParameter(gl.MAX_TEXTURE_SIZE);
                    this._caps.maxCubeMapTextureSize = gl.getParameter(gl.MAX_CUBE_MAP_TEXTURE_SIZE);
                    const extensions = gl.getSupportedExtensions();
                    let extStr = '';
                    if (extensions) {
                        for (const ext of extensions) {
                            extStr += `${ext} `;
                        }
                    }
                    const exts = webgl_swapchain_1.getExtensions(gl);
                    if (exts.WEBGL_debug_renderer_info) {
                        this._renderer = gl.getParameter(exts.WEBGL_debug_renderer_info.UNMASKED_RENDERER_WEBGL);
                        this._vendor = gl.getParameter(exts.WEBGL_debug_renderer_info.UNMASKED_VENDOR_WEBGL);
                    }
                    else {
                        this._renderer = gl.getParameter(gl.RENDERER);
                        this._vendor = gl.getParameter(gl.VENDOR);
                    }
                    const version = gl.getParameter(gl.VERSION);
                    this._features.fill(false);
                    if (exts.EXT_sRGB) {
                        this._features[define_28.Feature.FORMAT_SRGB] = true;
                    }
                    if (exts.EXT_blend_minmax) {
                        this._features[define_28.Feature.BLEND_MINMAX] = true;
                    }
                    if (exts.WEBGL_color_buffer_float) {
                        this._features[define_28.Feature.COLOR_FLOAT] = true;
                    }
                    if (exts.EXT_color_buffer_half_float) {
                        this._features[define_28.Feature.COLOR_HALF_FLOAT] = true;
                    }
                    if (exts.OES_texture_float) {
                        this._features[define_28.Feature.TEXTURE_FLOAT] = true;
                    }
                    if (exts.OES_texture_half_float) {
                        this._features[define_28.Feature.TEXTURE_HALF_FLOAT] = true;
                    }
                    if (exts.OES_texture_float_linear) {
                        this._features[define_28.Feature.TEXTURE_FLOAT_LINEAR] = true;
                    }
                    if (exts.OES_texture_half_float_linear) {
                        this._features[define_28.Feature.TEXTURE_HALF_FLOAT_LINEAR] = true;
                    }
                    this._features[define_28.Feature.FORMAT_RGB8] = true;
                    if (exts.OES_element_index_uint) {
                        this._features[define_28.Feature.ELEMENT_INDEX_UINT] = true;
                    }
                    if (exts.ANGLE_instanced_arrays) {
                        this._features[define_28.Feature.INSTANCED_ARRAYS] = true;
                    }
                    if (exts.WEBGL_draw_buffers) {
                        this._features[define_28.Feature.MULTIPLE_RENDER_TARGETS] = true;
                    }
                    let compressedFormat = '';
                    if (exts.WEBGL_compressed_texture_etc1) {
                        this._features[define_28.Feature.FORMAT_ETC1] = true;
                        compressedFormat += 'etc1 ';
                    }
                    if (exts.WEBGL_compressed_texture_etc) {
                        this._features[define_28.Feature.FORMAT_ETC2] = true;
                        compressedFormat += 'etc2 ';
                    }
                    if (exts.WEBGL_compressed_texture_s3tc) {
                        this._features[define_28.Feature.FORMAT_DXT] = true;
                        compressedFormat += 'dxt ';
                    }
                    if (exts.WEBGL_compressed_texture_pvrtc) {
                        this._features[define_28.Feature.FORMAT_PVRTC] = true;
                        compressedFormat += 'pvrtc ';
                    }
                    if (exts.WEBGL_compressed_texture_astc) {
                        this._features[define_28.Feature.FORMAT_ASTC] = true;
                        compressedFormat += 'astc ';
                    }
                    debug_3.debug('WebGL device initialized.');
                    debug_3.debug(`RENDERER: ${this._renderer}`);
                    debug_3.debug(`VENDOR: ${this._vendor}`);
                    debug_3.debug(`VERSION: ${version}`);
                    debug_3.debug(`COMPRESSED_FORMAT: ${compressedFormat}`);
                    debug_3.debug(`EXTENSIONS: ${extStr}`);
                    return true;
                }
                destroy() {
                    if (this._queue) {
                        this._queue.destroy();
                        this._queue = null;
                    }
                    if (this._cmdBuff) {
                        this._cmdBuff.destroy();
                        this._cmdBuff = null;
                    }
                }
                flushCommands(cmdBuffs) { }
                acquire(swapchains) { }
                present() {
                    const queue = this._queue;
                    this._numDrawCalls = queue.numDrawCalls;
                    this._numInstances = queue.numInstances;
                    this._numTris = queue.numTris;
                    queue.clear();
                }
                createCommandBuffer(info) {
                    // const Ctor = WebGLCommandBuffer; // opt to instant invocation
                    const Ctor = info.type === define_28.CommandBufferType.PRIMARY ? webgl_primary_command_buffer_1.WebGLPrimaryCommandBuffer : webgl_command_buffer_2.WebGLCommandBuffer;
                    const cmdBuff = new Ctor();
                    cmdBuff.initialize(info);
                    return cmdBuff;
                }
                createSwapchain(info) {
                    const swapchain = new webgl_swapchain_1.WebGLSwapchain();
                    this._swapchain = swapchain;
                    swapchain.initialize(info);
                    return swapchain;
                }
                createBuffer(info) {
                    const buffer = new webgl_buffer_1.WebGLBuffer();
                    buffer.initialize(info);
                    return buffer;
                }
                createTexture(info) {
                    const texture = new webgl_texture_2.WebGLTexture();
                    texture.initialize(info);
                    return texture;
                }
                createDescriptorSet(info) {
                    const descriptorSet = new webgl_descriptor_set_1.WebGLDescriptorSet();
                    descriptorSet.initialize(info);
                    return descriptorSet;
                }
                createShader(info) {
                    const shader = new webgl_shader_1.WebGLShader();
                    shader.initialize(info);
                    return shader;
                }
                createInputAssembler(info) {
                    const inputAssembler = new webgl_input_assembler_1.WebGLInputAssembler();
                    inputAssembler.initialize(info);
                    return inputAssembler;
                }
                createRenderPass(info) {
                    const renderPass = new webgl_render_pass_1.WebGLRenderPass();
                    renderPass.initialize(info);
                    return renderPass;
                }
                createFramebuffer(info) {
                    const framebuffer = new webgl_framebuffer_1.WebGLFramebuffer();
                    framebuffer.initialize(info);
                    return framebuffer;
                }
                createDescriptorSetLayout(info) {
                    const descriptorSetLayout = new webgl_descriptor_set_layout_1.WebGLDescriptorSetLayout();
                    descriptorSetLayout.initialize(info);
                    return descriptorSetLayout;
                }
                createPipelineLayout(info) {
                    const pipelineLayout = new webgl_pipeline_layout_1.WebGLPipelineLayout();
                    pipelineLayout.initialize(info);
                    return pipelineLayout;
                }
                createPipelineState(info) {
                    const pipelineState = new webgl_pipeline_state_1.WebGLPipelineState();
                    pipelineState.initialize(info);
                    return pipelineState;
                }
                createQueue(info) {
                    const queue = new webgl_queue_1.WebGLQueue();
                    queue.initialize(info);
                    return queue;
                }
                getSampler(info) {
                    const hash = sampler_2.Sampler.computeHash(info);
                    if (!this._samplers.has(hash)) {
                        this._samplers.set(hash, new webgl_sampler_1.WebGLSampler(info, hash));
                    }
                    return this._samplers.get(hash);
                }
                getGlobalBarrier(info) {
                    const hash = global_barrier_1.GlobalBarrier.computeHash(info);
                    if (!this._globalBarriers.has(hash)) {
                        this._globalBarriers.set(hash, new global_barrier_1.GlobalBarrier(info, hash));
                    }
                    return this._globalBarriers.get(hash);
                }
                getTextureBarrier(info) {
                    const hash = texture_barrier_1.TextureBarrier.computeHash(info);
                    if (!this._textureBarriers.has(hash)) {
                        this._textureBarriers.set(hash, new texture_barrier_1.TextureBarrier(info, hash));
                    }
                    return this._textureBarriers.get(hash);
                }
                copyBuffersToTexture(buffers, texture, regions) {
                    webgl_commands_9.WebGLCmdFuncCopyBuffersToTexture(this, buffers, texture.gpuTexture, regions);
                }
                copyTextureToBuffers(texture, buffers, regions) {
                    webgl_commands_9.WebGLCmdFuncCopyTextureToBuffers(this, texture.gpuTexture, buffers, regions);
                }
                copyTexImagesToTexture(texImages, texture, regions) {
                    webgl_commands_9.WebGLCmdFuncCopyTexImagesToTexture(this, texImages, texture.gpuTexture, regions);
                }
            };
            exports_47("WebGLDevice", WebGLDevice);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-define", [], function (exports_48, context_48) {
    "use strict";
    var WebGL2EXT, WebGL2DeviceManager;
    var __moduleName = context_48 && context_48.id;
    return {
        setters: [],
        execute: function () {
            // Extensions
            (function (WebGL2EXT) {
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGB_S3TC_DXT1_EXT"] = 33776] = "COMPRESSED_RGB_S3TC_DXT1_EXT";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_S3TC_DXT1_EXT"] = 33777] = "COMPRESSED_RGBA_S3TC_DXT1_EXT";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_S3TC_DXT3_EXT"] = 33778] = "COMPRESSED_RGBA_S3TC_DXT3_EXT";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_S3TC_DXT5_EXT"] = 33779] = "COMPRESSED_RGBA_S3TC_DXT5_EXT";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB_S3TC_DXT1_EXT"] = 35916] = "COMPRESSED_SRGB_S3TC_DXT1_EXT";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT"] = 35917] = "COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT"] = 35918] = "COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT"] = 35919] = "COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGB_PVRTC_4BPPV1_IMG"] = 35840] = "COMPRESSED_RGB_PVRTC_4BPPV1_IMG";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGB_PVRTC_2BPPV1_IMG"] = 35841] = "COMPRESSED_RGB_PVRTC_2BPPV1_IMG";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_PVRTC_4BPPV1_IMG"] = 35842] = "COMPRESSED_RGBA_PVRTC_4BPPV1_IMG";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_PVRTC_2BPPV1_IMG"] = 35843] = "COMPRESSED_RGBA_PVRTC_2BPPV1_IMG";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGB_ETC1_WEBGL"] = 36196] = "COMPRESSED_RGB_ETC1_WEBGL";
                WebGL2EXT[WebGL2EXT["COMPRESSED_R11_EAC"] = 37488] = "COMPRESSED_R11_EAC";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SIGNED_R11_EAC"] = 37489] = "COMPRESSED_SIGNED_R11_EAC";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RG11_EAC"] = 37490] = "COMPRESSED_RG11_EAC";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SIGNED_RG11_EAC"] = 37491] = "COMPRESSED_SIGNED_RG11_EAC";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGB8_ETC2"] = 37492] = "COMPRESSED_RGB8_ETC2";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ETC2"] = 37493] = "COMPRESSED_SRGB8_ETC2";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2"] = 37494] = "COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2"] = 37495] = "COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA8_ETC2_EAC"] = 37496] = "COMPRESSED_RGBA8_ETC2_EAC";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ETC2_EAC"] = 37497] = "COMPRESSED_SRGB8_ALPHA8_ETC2_EAC";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_4x4_KHR"] = 37808] = "COMPRESSED_RGBA_ASTC_4x4_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_5x4_KHR"] = 37809] = "COMPRESSED_RGBA_ASTC_5x4_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_5x5_KHR"] = 37810] = "COMPRESSED_RGBA_ASTC_5x5_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_6x5_KHR"] = 37811] = "COMPRESSED_RGBA_ASTC_6x5_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_6x6_KHR"] = 37812] = "COMPRESSED_RGBA_ASTC_6x6_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_8x5_KHR"] = 37813] = "COMPRESSED_RGBA_ASTC_8x5_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_8x6_KHR"] = 37814] = "COMPRESSED_RGBA_ASTC_8x6_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_8x8_KHR"] = 37815] = "COMPRESSED_RGBA_ASTC_8x8_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_10x5_KHR"] = 37816] = "COMPRESSED_RGBA_ASTC_10x5_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_10x6_KHR"] = 37817] = "COMPRESSED_RGBA_ASTC_10x6_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_10x8_KHR"] = 37818] = "COMPRESSED_RGBA_ASTC_10x8_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_10x10_KHR"] = 37819] = "COMPRESSED_RGBA_ASTC_10x10_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_12x10_KHR"] = 37820] = "COMPRESSED_RGBA_ASTC_12x10_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_RGBA_ASTC_12x12_KHR"] = 37821] = "COMPRESSED_RGBA_ASTC_12x12_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR"] = 37840] = "COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR"] = 37841] = "COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR"] = 37842] = "COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR"] = 37843] = "COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR"] = 37844] = "COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR"] = 37845] = "COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR"] = 37846] = "COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR"] = 37847] = "COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR"] = 37848] = "COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR"] = 37849] = "COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR"] = 37850] = "COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR"] = 37851] = "COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR"] = 37852] = "COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR";
                WebGL2EXT[WebGL2EXT["COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR"] = 37853] = "COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR";
            })(WebGL2EXT || (WebGL2EXT = {}));
            exports_48("WebGL2EXT", WebGL2EXT);
            // put the global instance here so that we won't have circular dependencies
            WebGL2DeviceManager = class WebGL2DeviceManager {
                static get instance() {
                    return WebGL2DeviceManager._instance;
                }
                static setInstance(instance) {
                    WebGL2DeviceManager._instance = instance;
                }
            };
            exports_48("WebGL2DeviceManager", WebGL2DeviceManager);
            WebGL2DeviceManager._instance = null;
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-command-allocator", ["engine/cocos/core/memop/cached-array", "engine/cocos/core/gfx/webgl2/webgl2-commands"], function (exports_49, context_49) {
    "use strict";
    var cached_array_3, webgl2_commands_1, WebGL2CommandPool, WebGL2CommandAllocator;
    var __moduleName = context_49 && context_49.id;
    return {
        setters: [
            function (cached_array_3_1) {
                cached_array_3 = cached_array_3_1;
            },
            function (webgl2_commands_1_1) {
                webgl2_commands_1 = webgl2_commands_1_1;
            }
        ],
        execute: function () {
            WebGL2CommandPool = class WebGL2CommandPool {
                constructor(Clazz, count) {
                    this._freeIdx = 0;
                    this._frees = new Array(count);
                    this._freeCmds = new cached_array_3.CachedArray(count);
                    for (let i = 0; i < count; ++i) {
                        this._frees[i] = new Clazz();
                    }
                    this._freeIdx = count - 1;
                }
                /*
                public alloc (clazz: new() => T): T {
                    return new clazz();
                }
                */
                alloc(Clazz) {
                    if (this._freeIdx < 0) {
                        const size = this._frees.length * 2;
                        const temp = this._frees;
                        this._frees = new Array(size);
                        const increase = size - temp.length;
                        for (let i = 0; i < increase; ++i) {
                            this._frees[i] = new Clazz();
                        }
                        for (let i = increase, j = 0; i < size; ++i, ++j) {
                            this._frees[i] = temp[j];
                        }
                        this._freeIdx += increase;
                    }
                    const cmd = this._frees[this._freeIdx];
                    this._frees[this._freeIdx--] = null;
                    ++cmd.refCount;
                    return cmd;
                }
                free(cmd) {
                    if (--cmd.refCount === 0) {
                        this._freeCmds.push(cmd);
                    }
                }
                freeCmds(cmds) {
                    // return ;
                    for (let i = 0; i < cmds.length; ++i) {
                        if (--cmds.array[i].refCount === 0) {
                            this._freeCmds.push(cmds.array[i]);
                        }
                    }
                }
                release() {
                    for (let i = 0; i < this._freeCmds.length; ++i) {
                        const cmd = this._freeCmds.array[i];
                        cmd.clear();
                        this._frees[++this._freeIdx] = cmd;
                    }
                    this._freeCmds.clear();
                }
            };
            exports_49("WebGL2CommandPool", WebGL2CommandPool);
            WebGL2CommandAllocator = class WebGL2CommandAllocator {
                constructor() {
                    this.beginRenderPassCmdPool = new WebGL2CommandPool(webgl2_commands_1.WebGL2CmdBeginRenderPass, 1);
                    this.bindStatesCmdPool = new WebGL2CommandPool(webgl2_commands_1.WebGL2CmdBindStates, 1);
                    this.drawCmdPool = new WebGL2CommandPool(webgl2_commands_1.WebGL2CmdDraw, 1);
                    this.updateBufferCmdPool = new WebGL2CommandPool(webgl2_commands_1.WebGL2CmdUpdateBuffer, 1);
                    this.copyBufferToTextureCmdPool = new WebGL2CommandPool(webgl2_commands_1.WebGL2CmdCopyBufferToTexture, 1);
                }
                clearCmds(cmdPackage) {
                    if (cmdPackage.beginRenderPassCmds.length) {
                        this.beginRenderPassCmdPool.freeCmds(cmdPackage.beginRenderPassCmds);
                        cmdPackage.beginRenderPassCmds.clear();
                    }
                    if (cmdPackage.bindStatesCmds.length) {
                        this.bindStatesCmdPool.freeCmds(cmdPackage.bindStatesCmds);
                        cmdPackage.bindStatesCmds.clear();
                    }
                    if (cmdPackage.drawCmds.length) {
                        this.drawCmdPool.freeCmds(cmdPackage.drawCmds);
                        cmdPackage.drawCmds.clear();
                    }
                    if (cmdPackage.updateBufferCmds.length) {
                        this.updateBufferCmdPool.freeCmds(cmdPackage.updateBufferCmds);
                        cmdPackage.updateBufferCmds.clear();
                    }
                    if (cmdPackage.copyBufferToTextureCmds.length) {
                        this.copyBufferToTextureCmdPool.freeCmds(cmdPackage.copyBufferToTextureCmds);
                        cmdPackage.copyBufferToTextureCmds.clear();
                    }
                    cmdPackage.cmds.clear();
                }
                releaseCmds() {
                    this.beginRenderPassCmdPool.release();
                    this.bindStatesCmdPool.release();
                    this.drawCmdPool.release();
                    this.updateBufferCmdPool.release();
                    this.copyBufferToTextureCmdPool.release();
                }
            };
            exports_49("WebGL2CommandAllocator", WebGL2CommandAllocator);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-gpu-objects", ["engine/cocos/core/math/bits"], function (exports_50, context_50) {
    "use strict";
    var bits_2, WebGL2IndirectDrawInfos;
    var __moduleName = context_50 && context_50.id;
    return {
        setters: [
            function (bits_2_1) {
                bits_2 = bits_2_1;
            }
        ],
        execute: function () {
            WebGL2IndirectDrawInfos = class WebGL2IndirectDrawInfos {
                constructor() {
                    this.drawCount = 0;
                    this.drawByIndex = false;
                    this.instancedDraw = false;
                    this._capacity = 4;
                    this.counts = new Int32Array(this._capacity);
                    this.offsets = new Int32Array(this._capacity);
                    this.instances = new Int32Array(this._capacity);
                    this.byteOffsets = new Int32Array(this._capacity);
                }
                clearDraws() {
                    this.drawCount = 0;
                    this.drawByIndex = false;
                    this.instancedDraw = false;
                }
                setDrawInfo(idx, info) {
                    this._ensureCapacity(idx);
                    this.drawByIndex = info.indexCount > 0;
                    this.instancedDraw = info.instanceCount > 1;
                    this.drawCount = Math.max(idx + 1, this.drawCount);
                    if (this.drawByIndex) {
                        this.counts[idx] = info.indexCount;
                        this.offsets[idx] = info.firstIndex;
                    }
                    else {
                        this.counts[idx] = info.vertexCount;
                        this.offsets[idx] = info.firstVertex;
                    }
                    this.instances[idx] = Math.max(1, info.instanceCount);
                }
                _ensureCapacity(target) {
                    if (this._capacity > target)
                        return;
                    this._capacity = bits_2.nextPow2(target);
                    const counts = new Int32Array(this._capacity);
                    const offsets = new Int32Array(this._capacity);
                    const instances = new Int32Array(this._capacity);
                    this.byteOffsets = new Int32Array(this._capacity);
                    counts.set(this.counts);
                    offsets.set(this.offsets);
                    instances.set(this.instances);
                    this.counts = counts;
                    this.offsets = offsets;
                    this.instances = instances;
                }
            };
            exports_50("WebGL2IndirectDrawInfos", WebGL2IndirectDrawInfos);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-commands", ["engine/cocos/core/memop/cached-array", "engine/cocos/core/platform/debug", "engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_51, context_51) {
    "use strict";
    var cached_array_4, debug_4, define_29, webgl2_define_1, WebGLWraps, _f32v4, WebGLCmpFuncs, WebGLStencilOps, WebGLBlendOps, WebGLBlendFactors, WebGL2Cmd, WebGL2CmdObject, WebGL2CmdBeginRenderPass, WebGL2CmdBindStates, WebGL2CmdDraw, WebGL2CmdUpdateBuffer, WebGL2CmdCopyBufferToTexture, WebGL2CmdPackage, gfxStateCache, cmdIds;
    var __moduleName = context_51 && context_51.id;
    function CmpF32NotEuqal(a, b) {
        const c = a - b;
        return (c > 0.000001 || c < -0.000001);
    }
    function GFXFormatToWebGLType(format, gl) {
        switch (format) {
            case define_29.Format.R8: return gl.UNSIGNED_BYTE;
            case define_29.Format.R8SN: return gl.BYTE;
            case define_29.Format.R8UI: return gl.UNSIGNED_BYTE;
            case define_29.Format.R8I: return gl.BYTE;
            case define_29.Format.R16F: return gl.HALF_FLOAT;
            case define_29.Format.R16UI: return gl.UNSIGNED_SHORT;
            case define_29.Format.R16I: return gl.SHORT;
            case define_29.Format.R32F: return gl.FLOAT;
            case define_29.Format.R32UI: return gl.UNSIGNED_INT;
            case define_29.Format.R32I: return gl.INT;
            case define_29.Format.RG8: return gl.UNSIGNED_BYTE;
            case define_29.Format.RG8SN: return gl.BYTE;
            case define_29.Format.RG8UI: return gl.UNSIGNED_BYTE;
            case define_29.Format.RG8I: return gl.BYTE;
            case define_29.Format.RG16F: return gl.HALF_FLOAT;
            case define_29.Format.RG16UI: return gl.UNSIGNED_SHORT;
            case define_29.Format.RG16I: return gl.SHORT;
            case define_29.Format.RG32F: return gl.FLOAT;
            case define_29.Format.RG32UI: return gl.UNSIGNED_INT;
            case define_29.Format.RG32I: return gl.INT;
            case define_29.Format.RGB8: return gl.UNSIGNED_BYTE;
            case define_29.Format.SRGB8: return gl.UNSIGNED_BYTE;
            case define_29.Format.RGB8SN: return gl.BYTE;
            case define_29.Format.RGB8UI: return gl.UNSIGNED_BYTE;
            case define_29.Format.RGB8I: return gl.BYTE;
            case define_29.Format.RGB16F: return gl.HALF_FLOAT;
            case define_29.Format.RGB16UI: return gl.UNSIGNED_SHORT;
            case define_29.Format.RGB16I: return gl.SHORT;
            case define_29.Format.RGB32F: return gl.FLOAT;
            case define_29.Format.RGB32UI: return gl.UNSIGNED_INT;
            case define_29.Format.RGB32I: return gl.INT;
            case define_29.Format.BGRA8: return gl.UNSIGNED_BYTE;
            case define_29.Format.RGBA8: return gl.UNSIGNED_BYTE;
            case define_29.Format.SRGB8_A8: return gl.UNSIGNED_BYTE;
            case define_29.Format.RGBA8SN: return gl.BYTE;
            case define_29.Format.RGBA8UI: return gl.UNSIGNED_BYTE;
            case define_29.Format.RGBA8I: return gl.BYTE;
            case define_29.Format.RGBA16F: return gl.HALF_FLOAT;
            case define_29.Format.RGBA16UI: return gl.UNSIGNED_SHORT;
            case define_29.Format.RGBA16I: return gl.SHORT;
            case define_29.Format.RGBA32F: return gl.FLOAT;
            case define_29.Format.RGBA32UI: return gl.UNSIGNED_INT;
            case define_29.Format.RGBA32I: return gl.INT;
            case define_29.Format.R5G6B5: return gl.UNSIGNED_SHORT_5_6_5;
            case define_29.Format.R11G11B10F: return gl.UNSIGNED_INT_10F_11F_11F_REV;
            case define_29.Format.RGB5A1: return gl.UNSIGNED_SHORT_5_5_5_1;
            case define_29.Format.RGBA4: return gl.UNSIGNED_SHORT_4_4_4_4;
            case define_29.Format.RGB10A2: return gl.UNSIGNED_INT_2_10_10_10_REV;
            case define_29.Format.RGB10A2UI: return gl.UNSIGNED_INT_2_10_10_10_REV;
            case define_29.Format.RGB9E5: return gl.FLOAT;
            case define_29.Format.DEPTH: return gl.FLOAT;
            case define_29.Format.DEPTH_STENCIL: return gl.FLOAT_32_UNSIGNED_INT_24_8_REV;
            case define_29.Format.BC1: return gl.UNSIGNED_BYTE;
            case define_29.Format.BC1_SRGB: return gl.UNSIGNED_BYTE;
            case define_29.Format.BC2: return gl.UNSIGNED_BYTE;
            case define_29.Format.BC2_SRGB: return gl.UNSIGNED_BYTE;
            case define_29.Format.BC3: return gl.UNSIGNED_BYTE;
            case define_29.Format.BC3_SRGB: return gl.UNSIGNED_BYTE;
            case define_29.Format.BC4: return gl.UNSIGNED_BYTE;
            case define_29.Format.BC4_SNORM: return gl.BYTE;
            case define_29.Format.BC5: return gl.UNSIGNED_BYTE;
            case define_29.Format.BC5_SNORM: return gl.BYTE;
            case define_29.Format.BC6H_SF16: return gl.FLOAT;
            case define_29.Format.BC6H_UF16: return gl.FLOAT;
            case define_29.Format.BC7: return gl.UNSIGNED_BYTE;
            case define_29.Format.BC7_SRGB: return gl.UNSIGNED_BYTE;
            case define_29.Format.ETC_RGB8: return gl.UNSIGNED_BYTE;
            case define_29.Format.ETC2_RGB8: return gl.UNSIGNED_BYTE;
            case define_29.Format.ETC2_SRGB8: return gl.UNSIGNED_BYTE;
            case define_29.Format.ETC2_RGB8_A1: return gl.UNSIGNED_BYTE;
            case define_29.Format.ETC2_SRGB8_A1: return gl.UNSIGNED_BYTE;
            case define_29.Format.EAC_R11: return gl.UNSIGNED_BYTE;
            case define_29.Format.EAC_R11SN: return gl.BYTE;
            case define_29.Format.EAC_RG11: return gl.UNSIGNED_BYTE;
            case define_29.Format.EAC_RG11SN: return gl.BYTE;
            case define_29.Format.PVRTC_RGB2: return gl.UNSIGNED_BYTE;
            case define_29.Format.PVRTC_RGBA2: return gl.UNSIGNED_BYTE;
            case define_29.Format.PVRTC_RGB4: return gl.UNSIGNED_BYTE;
            case define_29.Format.PVRTC_RGBA4: return gl.UNSIGNED_BYTE;
            case define_29.Format.PVRTC2_2BPP: return gl.UNSIGNED_BYTE;
            case define_29.Format.PVRTC2_4BPP: return gl.UNSIGNED_BYTE;
            case define_29.Format.ASTC_RGBA_4X4:
            case define_29.Format.ASTC_RGBA_5X4:
            case define_29.Format.ASTC_RGBA_5X5:
            case define_29.Format.ASTC_RGBA_6X5:
            case define_29.Format.ASTC_RGBA_6X6:
            case define_29.Format.ASTC_RGBA_8X5:
            case define_29.Format.ASTC_RGBA_8X6:
            case define_29.Format.ASTC_RGBA_8X8:
            case define_29.Format.ASTC_RGBA_10X5:
            case define_29.Format.ASTC_RGBA_10X6:
            case define_29.Format.ASTC_RGBA_10X8:
            case define_29.Format.ASTC_RGBA_10X10:
            case define_29.Format.ASTC_RGBA_12X10:
            case define_29.Format.ASTC_RGBA_12X12:
            case define_29.Format.ASTC_SRGBA_4X4:
            case define_29.Format.ASTC_SRGBA_5X4:
            case define_29.Format.ASTC_SRGBA_5X5:
            case define_29.Format.ASTC_SRGBA_6X5:
            case define_29.Format.ASTC_SRGBA_6X6:
            case define_29.Format.ASTC_SRGBA_8X5:
            case define_29.Format.ASTC_SRGBA_8X6:
            case define_29.Format.ASTC_SRGBA_8X8:
            case define_29.Format.ASTC_SRGBA_10X5:
            case define_29.Format.ASTC_SRGBA_10X6:
            case define_29.Format.ASTC_SRGBA_10X8:
            case define_29.Format.ASTC_SRGBA_10X10:
            case define_29.Format.ASTC_SRGBA_12X10:
            case define_29.Format.ASTC_SRGBA_12X12:
                return gl.UNSIGNED_BYTE;
            default: {
                return gl.UNSIGNED_BYTE;
            }
        }
    }
    exports_51("GFXFormatToWebGLType", GFXFormatToWebGLType);
    function GFXFormatToWebGLInternalFormat(format, gl) {
        switch (format) {
            case define_29.Format.A8: return gl.ALPHA;
            case define_29.Format.L8: return gl.LUMINANCE;
            case define_29.Format.LA8: return gl.LUMINANCE_ALPHA;
            case define_29.Format.R8: return gl.R8;
            case define_29.Format.R8SN: return gl.R8_SNORM;
            case define_29.Format.R8UI: return gl.R8UI;
            case define_29.Format.R8I: return gl.R8I;
            case define_29.Format.RG8: return gl.RG8;
            case define_29.Format.RG8SN: return gl.RG8_SNORM;
            case define_29.Format.RG8UI: return gl.RG8UI;
            case define_29.Format.RG8I: return gl.RG8I;
            case define_29.Format.RGB8: return gl.RGB8;
            case define_29.Format.RGB8SN: return gl.RGB8_SNORM;
            case define_29.Format.RGB8UI: return gl.RGB8UI;
            case define_29.Format.RGB8I: return gl.RGB8I;
            case define_29.Format.BGRA8: return gl.RGBA8;
            case define_29.Format.RGBA8: return gl.RGBA8;
            case define_29.Format.RGBA8SN: return gl.RGBA8_SNORM;
            case define_29.Format.RGBA8UI: return gl.RGBA8UI;
            case define_29.Format.RGBA8I: return gl.RGBA8I;
            case define_29.Format.R16I: return gl.R16I;
            case define_29.Format.R16UI: return gl.R16UI;
            case define_29.Format.R16F: return gl.R16F;
            case define_29.Format.RG16I: return gl.RG16I;
            case define_29.Format.RG16UI: return gl.RG16UI;
            case define_29.Format.RG16F: return gl.RG16F;
            case define_29.Format.RGB16I: return gl.RGB16I;
            case define_29.Format.RGB16UI: return gl.RGB16UI;
            case define_29.Format.RGB16F: return gl.RGB16F;
            case define_29.Format.RGBA16I: return gl.RGBA16I;
            case define_29.Format.RGBA16UI: return gl.RGBA16UI;
            case define_29.Format.RGBA16F: return gl.RGBA16F;
            case define_29.Format.R32I: return gl.R32I;
            case define_29.Format.R32UI: return gl.R32UI;
            case define_29.Format.R32F: return gl.R32F;
            case define_29.Format.RG32I: return gl.RG32I;
            case define_29.Format.RG32UI: return gl.RG32UI;
            case define_29.Format.RG32F: return gl.RG32F;
            case define_29.Format.RGB32I: return gl.RGB32I;
            case define_29.Format.RGB32UI: return gl.RGB32UI;
            case define_29.Format.RGB32F: return gl.RGB32F;
            case define_29.Format.RGBA32I: return gl.RGBA32I;
            case define_29.Format.RGBA32UI: return gl.RGBA32UI;
            case define_29.Format.RGBA32F: return gl.RGBA32F;
            case define_29.Format.R5G6B5: return gl.RGB565;
            case define_29.Format.RGB5A1: return gl.RGB5_A1;
            case define_29.Format.RGBA4: return gl.RGBA4;
            case define_29.Format.SRGB8: return gl.SRGB8;
            case define_29.Format.SRGB8_A8: return gl.SRGB8_ALPHA8;
            case define_29.Format.RGB10A2: return gl.RGB10_A2;
            case define_29.Format.RGB10A2UI: return gl.RGB10_A2UI;
            case define_29.Format.R11G11B10F: return gl.R11F_G11F_B10F;
            case define_29.Format.DEPTH: return gl.DEPTH_COMPONENT32F;
            case define_29.Format.DEPTH_STENCIL: return gl.DEPTH32F_STENCIL8;
            case define_29.Format.BC1: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_S3TC_DXT1_EXT;
            case define_29.Format.BC1_ALPHA: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case define_29.Format.BC1_SRGB: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB_S3TC_DXT1_EXT;
            case define_29.Format.BC1_SRGB_ALPHA: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
            case define_29.Format.BC2: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case define_29.Format.BC2_SRGB: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
            case define_29.Format.BC3: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_S3TC_DXT5_EXT;
            case define_29.Format.BC3_SRGB: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
            case define_29.Format.ETC_RGB8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_ETC1_WEBGL;
            case define_29.Format.ETC2_RGB8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB8_ETC2;
            case define_29.Format.ETC2_SRGB8: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ETC2;
            case define_29.Format.ETC2_RGB8_A1: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            case define_29.Format.ETC2_SRGB8_A1: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            case define_29.Format.ETC2_RGBA8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA8_ETC2_EAC;
            case define_29.Format.ETC2_SRGB8_A8: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
            case define_29.Format.EAC_R11: return webgl2_define_1.WebGL2EXT.COMPRESSED_R11_EAC;
            case define_29.Format.EAC_R11SN: return webgl2_define_1.WebGL2EXT.COMPRESSED_SIGNED_R11_EAC;
            case define_29.Format.EAC_RG11: return webgl2_define_1.WebGL2EXT.COMPRESSED_RG11_EAC;
            case define_29.Format.EAC_RG11SN: return webgl2_define_1.WebGL2EXT.COMPRESSED_SIGNED_RG11_EAC;
            case define_29.Format.PVRTC_RGB2: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
            case define_29.Format.PVRTC_RGBA2: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
            case define_29.Format.PVRTC_RGB4: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
            case define_29.Format.PVRTC_RGBA4: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
            case define_29.Format.ASTC_RGBA_4X4: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_4x4_KHR;
            case define_29.Format.ASTC_RGBA_5X4: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_5x4_KHR;
            case define_29.Format.ASTC_RGBA_5X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_5x5_KHR;
            case define_29.Format.ASTC_RGBA_6X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_6x5_KHR;
            case define_29.Format.ASTC_RGBA_6X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_6x6_KHR;
            case define_29.Format.ASTC_RGBA_8X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_8x5_KHR;
            case define_29.Format.ASTC_RGBA_8X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_8x6_KHR;
            case define_29.Format.ASTC_RGBA_8X8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_8x8_KHR;
            case define_29.Format.ASTC_RGBA_10X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_10x5_KHR;
            case define_29.Format.ASTC_RGBA_10X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_10x6_KHR;
            case define_29.Format.ASTC_RGBA_10X8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_10x8_KHR;
            case define_29.Format.ASTC_RGBA_10X10: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_10x10_KHR;
            case define_29.Format.ASTC_RGBA_12X10: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_12x10_KHR;
            case define_29.Format.ASTC_RGBA_12X12: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_12x12_KHR;
            case define_29.Format.ASTC_SRGBA_4X4: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
            case define_29.Format.ASTC_SRGBA_5X4: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
            case define_29.Format.ASTC_SRGBA_5X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
            case define_29.Format.ASTC_SRGBA_6X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
            case define_29.Format.ASTC_SRGBA_6X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
            case define_29.Format.ASTC_SRGBA_8X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
            case define_29.Format.ASTC_SRGBA_8X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
            case define_29.Format.ASTC_SRGBA_8X8: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
            case define_29.Format.ASTC_SRGBA_10X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
            case define_29.Format.ASTC_SRGBA_10X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
            case define_29.Format.ASTC_SRGBA_10X8: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
            case define_29.Format.ASTC_SRGBA_10X10: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
            case define_29.Format.ASTC_SRGBA_12X10: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
            case define_29.Format.ASTC_SRGBA_12X12: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
            default: {
                console.error('Unsupported Format, convert to WebGL internal format failed.');
                return gl.RGBA;
            }
        }
    }
    exports_51("GFXFormatToWebGLInternalFormat", GFXFormatToWebGLInternalFormat);
    function GFXFormatToWebGLFormat(format, gl) {
        switch (format) {
            case define_29.Format.A8: return gl.ALPHA;
            case define_29.Format.L8: return gl.LUMINANCE;
            case define_29.Format.LA8: return gl.LUMINANCE_ALPHA;
            case define_29.Format.R8:
            case define_29.Format.R8SN: return gl.RED;
            case define_29.Format.R8UI:
            case define_29.Format.R8I: return gl.RED;
            case define_29.Format.RG8:
            case define_29.Format.RG8SN:
            case define_29.Format.RG8UI:
            case define_29.Format.RG8I: return gl.RG;
            case define_29.Format.RGB8:
            case define_29.Format.RGB8SN:
            case define_29.Format.RGB8UI:
            case define_29.Format.RGB8I: return gl.RGB;
            case define_29.Format.BGRA8:
            case define_29.Format.RGBA8:
            case define_29.Format.RGBA8SN:
            case define_29.Format.RGBA8UI:
            case define_29.Format.RGBA8I: return gl.RGBA;
            case define_29.Format.R16UI:
            case define_29.Format.R16I:
            case define_29.Format.R16F: return gl.RED;
            case define_29.Format.RG16UI:
            case define_29.Format.RG16I:
            case define_29.Format.RG16F: return gl.RG;
            case define_29.Format.RGB16UI:
            case define_29.Format.RGB16I:
            case define_29.Format.RGB16F: return gl.RGB;
            case define_29.Format.RGBA16UI:
            case define_29.Format.RGBA16I:
            case define_29.Format.RGBA16F: return gl.RGBA;
            case define_29.Format.R32UI:
            case define_29.Format.R32I:
            case define_29.Format.R32F: return gl.RED;
            case define_29.Format.RG32UI:
            case define_29.Format.RG32I:
            case define_29.Format.RG32F: return gl.RG;
            case define_29.Format.RGB32UI:
            case define_29.Format.RGB32I:
            case define_29.Format.RGB32F: return gl.RGB;
            case define_29.Format.RGBA32UI:
            case define_29.Format.RGBA32I:
            case define_29.Format.RGBA32F: return gl.RGBA;
            case define_29.Format.RGB10A2: return gl.RGBA;
            case define_29.Format.R11G11B10F: return gl.RGB;
            case define_29.Format.R5G6B5: return gl.RGB;
            case define_29.Format.RGB5A1: return gl.RGBA;
            case define_29.Format.RGBA4: return gl.RGBA;
            case define_29.Format.SRGB8: return gl.RGB;
            case define_29.Format.SRGB8_A8: return gl.RGBA;
            case define_29.Format.DEPTH: return gl.DEPTH_COMPONENT;
            case define_29.Format.DEPTH_STENCIL: return gl.DEPTH_STENCIL;
            case define_29.Format.BC1: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_S3TC_DXT1_EXT;
            case define_29.Format.BC1_ALPHA: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case define_29.Format.BC1_SRGB: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB_S3TC_DXT1_EXT;
            case define_29.Format.BC1_SRGB_ALPHA: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
            case define_29.Format.BC2: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case define_29.Format.BC2_SRGB: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
            case define_29.Format.BC3: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_S3TC_DXT5_EXT;
            case define_29.Format.BC3_SRGB: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;
            case define_29.Format.ETC_RGB8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_ETC1_WEBGL;
            case define_29.Format.ETC2_RGB8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB8_ETC2;
            case define_29.Format.ETC2_SRGB8: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ETC2;
            case define_29.Format.ETC2_RGB8_A1: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            case define_29.Format.ETC2_SRGB8_A1: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            case define_29.Format.ETC2_RGBA8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA8_ETC2_EAC;
            case define_29.Format.ETC2_SRGB8_A8: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
            case define_29.Format.EAC_R11: return webgl2_define_1.WebGL2EXT.COMPRESSED_R11_EAC;
            case define_29.Format.EAC_R11SN: return webgl2_define_1.WebGL2EXT.COMPRESSED_SIGNED_R11_EAC;
            case define_29.Format.EAC_RG11: return webgl2_define_1.WebGL2EXT.COMPRESSED_RG11_EAC;
            case define_29.Format.EAC_RG11SN: return webgl2_define_1.WebGL2EXT.COMPRESSED_SIGNED_RG11_EAC;
            case define_29.Format.PVRTC_RGB2: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
            case define_29.Format.PVRTC_RGBA2: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
            case define_29.Format.PVRTC_RGB4: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
            case define_29.Format.PVRTC_RGBA4: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
            case define_29.Format.ASTC_RGBA_4X4: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_4x4_KHR;
            case define_29.Format.ASTC_RGBA_5X4: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_5x4_KHR;
            case define_29.Format.ASTC_RGBA_5X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_5x5_KHR;
            case define_29.Format.ASTC_RGBA_6X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_6x5_KHR;
            case define_29.Format.ASTC_RGBA_6X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_6x6_KHR;
            case define_29.Format.ASTC_RGBA_8X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_8x5_KHR;
            case define_29.Format.ASTC_RGBA_8X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_8x6_KHR;
            case define_29.Format.ASTC_RGBA_8X8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_8x8_KHR;
            case define_29.Format.ASTC_RGBA_10X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_10x5_KHR;
            case define_29.Format.ASTC_RGBA_10X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_10x6_KHR;
            case define_29.Format.ASTC_RGBA_10X8: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_10x8_KHR;
            case define_29.Format.ASTC_RGBA_10X10: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_10x10_KHR;
            case define_29.Format.ASTC_RGBA_12X10: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_12x10_KHR;
            case define_29.Format.ASTC_RGBA_12X12: return webgl2_define_1.WebGL2EXT.COMPRESSED_RGBA_ASTC_12x12_KHR;
            case define_29.Format.ASTC_SRGBA_4X4: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
            case define_29.Format.ASTC_SRGBA_5X4: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
            case define_29.Format.ASTC_SRGBA_5X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
            case define_29.Format.ASTC_SRGBA_6X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
            case define_29.Format.ASTC_SRGBA_6X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
            case define_29.Format.ASTC_SRGBA_8X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
            case define_29.Format.ASTC_SRGBA_8X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
            case define_29.Format.ASTC_SRGBA_8X8: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
            case define_29.Format.ASTC_SRGBA_10X5: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
            case define_29.Format.ASTC_SRGBA_10X6: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
            case define_29.Format.ASTC_SRGBA_10X8: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
            case define_29.Format.ASTC_SRGBA_10X10: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
            case define_29.Format.ASTC_SRGBA_12X10: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
            case define_29.Format.ASTC_SRGBA_12X12: return webgl2_define_1.WebGL2EXT.COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
            default: {
                console.error('Unsupported Format, convert to WebGL format failed.');
                return gl.RGBA;
            }
        }
    }
    exports_51("GFXFormatToWebGLFormat", GFXFormatToWebGLFormat);
    function GFXTypeToWebGLType(type, gl) {
        switch (type) {
            case define_29.Type.BOOL: return gl.BOOL;
            case define_29.Type.BOOL2: return gl.BOOL_VEC2;
            case define_29.Type.BOOL3: return gl.BOOL_VEC3;
            case define_29.Type.BOOL4: return gl.BOOL_VEC4;
            case define_29.Type.INT: return gl.INT;
            case define_29.Type.INT2: return gl.INT_VEC2;
            case define_29.Type.INT3: return gl.INT_VEC3;
            case define_29.Type.INT4: return gl.INT_VEC4;
            case define_29.Type.UINT: return gl.UNSIGNED_INT;
            case define_29.Type.FLOAT: return gl.FLOAT;
            case define_29.Type.FLOAT2: return gl.FLOAT_VEC2;
            case define_29.Type.FLOAT3: return gl.FLOAT_VEC3;
            case define_29.Type.FLOAT4: return gl.FLOAT_VEC4;
            case define_29.Type.MAT2: return gl.FLOAT_MAT2;
            case define_29.Type.MAT2X3: return gl.FLOAT_MAT2x3;
            case define_29.Type.MAT2X4: return gl.FLOAT_MAT2x4;
            case define_29.Type.MAT3X2: return gl.FLOAT_MAT3x2;
            case define_29.Type.MAT3: return gl.FLOAT_MAT3;
            case define_29.Type.MAT3X4: return gl.FLOAT_MAT3x4;
            case define_29.Type.MAT4X2: return gl.FLOAT_MAT4x2;
            case define_29.Type.MAT4X3: return gl.FLOAT_MAT4x3;
            case define_29.Type.MAT4: return gl.FLOAT_MAT4;
            case define_29.Type.SAMPLER2D: return gl.SAMPLER_2D;
            case define_29.Type.SAMPLER2D_ARRAY: return gl.SAMPLER_2D_ARRAY;
            case define_29.Type.SAMPLER3D: return gl.SAMPLER_3D;
            case define_29.Type.SAMPLER_CUBE: return gl.SAMPLER_CUBE;
            default: {
                console.error('Unsupported GLType, convert to GL type failed.');
                return define_29.Type.UNKNOWN;
            }
        }
    }
    function WebGLTypeToGFXType(glType, gl) {
        switch (glType) {
            case gl.BOOL: return define_29.Type.BOOL;
            case gl.BOOL_VEC2: return define_29.Type.BOOL2;
            case gl.BOOL_VEC3: return define_29.Type.BOOL3;
            case gl.BOOL_VEC4: return define_29.Type.BOOL4;
            case gl.INT: return define_29.Type.INT;
            case gl.INT_VEC2: return define_29.Type.INT2;
            case gl.INT_VEC3: return define_29.Type.INT3;
            case gl.INT_VEC4: return define_29.Type.INT4;
            case gl.UNSIGNED_INT: return define_29.Type.UINT;
            case gl.UNSIGNED_INT_VEC2: return define_29.Type.UINT2;
            case gl.UNSIGNED_INT_VEC3: return define_29.Type.UINT3;
            case gl.UNSIGNED_INT_VEC4: return define_29.Type.UINT4;
            case gl.FLOAT: return define_29.Type.FLOAT;
            case gl.FLOAT_VEC2: return define_29.Type.FLOAT2;
            case gl.FLOAT_VEC3: return define_29.Type.FLOAT3;
            case gl.FLOAT_VEC4: return define_29.Type.FLOAT4;
            case gl.FLOAT_MAT2: return define_29.Type.MAT2;
            case gl.FLOAT_MAT2x3: return define_29.Type.MAT2X3;
            case gl.FLOAT_MAT2x4: return define_29.Type.MAT2X4;
            case gl.FLOAT_MAT3x2: return define_29.Type.MAT3X2;
            case gl.FLOAT_MAT3: return define_29.Type.MAT3;
            case gl.FLOAT_MAT3x4: return define_29.Type.MAT3X4;
            case gl.FLOAT_MAT4x2: return define_29.Type.MAT4X2;
            case gl.FLOAT_MAT4x3: return define_29.Type.MAT4X3;
            case gl.FLOAT_MAT4: return define_29.Type.MAT4;
            case gl.SAMPLER_2D: return define_29.Type.SAMPLER2D;
            case gl.SAMPLER_2D_ARRAY: return define_29.Type.SAMPLER2D_ARRAY;
            case gl.SAMPLER_3D: return define_29.Type.SAMPLER3D;
            case gl.SAMPLER_CUBE: return define_29.Type.SAMPLER_CUBE;
            default: {
                console.error('Unsupported GLType, convert to Type failed.');
                return define_29.Type.UNKNOWN;
            }
        }
    }
    function WebGLGetTypeSize(glType, gl) {
        switch (glType) {
            case gl.BOOL: return 4;
            case gl.BOOL_VEC2: return 8;
            case gl.BOOL_VEC3: return 12;
            case gl.BOOL_VEC4: return 16;
            case gl.INT: return 4;
            case gl.INT_VEC2: return 8;
            case gl.INT_VEC3: return 12;
            case gl.INT_VEC4: return 16;
            case gl.UNSIGNED_INT: return 4;
            case gl.UNSIGNED_INT_VEC2: return 8;
            case gl.UNSIGNED_INT_VEC3: return 12;
            case gl.UNSIGNED_INT_VEC4: return 16;
            case gl.FLOAT: return 4;
            case gl.FLOAT_VEC2: return 8;
            case gl.FLOAT_VEC3: return 12;
            case gl.FLOAT_VEC4: return 16;
            case gl.FLOAT_MAT2: return 16;
            case gl.FLOAT_MAT2x3: return 24;
            case gl.FLOAT_MAT2x4: return 32;
            case gl.FLOAT_MAT3x2: return 24;
            case gl.FLOAT_MAT3: return 36;
            case gl.FLOAT_MAT3x4: return 48;
            case gl.FLOAT_MAT4x2: return 32;
            case gl.FLOAT_MAT4x3: return 48;
            case gl.FLOAT_MAT4: return 64;
            case gl.SAMPLER_2D: return 4;
            case gl.SAMPLER_2D_ARRAY: return 4;
            case gl.SAMPLER_2D_ARRAY_SHADOW: return 4;
            case gl.SAMPLER_3D: return 4;
            case gl.SAMPLER_CUBE: return 4;
            case gl.INT_SAMPLER_2D: return 4;
            case gl.INT_SAMPLER_2D_ARRAY: return 4;
            case gl.INT_SAMPLER_3D: return 4;
            case gl.INT_SAMPLER_CUBE: return 4;
            case gl.UNSIGNED_INT_SAMPLER_2D: return 4;
            case gl.UNSIGNED_INT_SAMPLER_2D_ARRAY: return 4;
            case gl.UNSIGNED_INT_SAMPLER_3D: return 4;
            case gl.UNSIGNED_INT_SAMPLER_CUBE: return 4;
            default: {
                console.error('Unsupported GLType, get type failed.');
                return 0;
            }
        }
    }
    function WebGLGetComponentCount(glType, gl) {
        switch (glType) {
            case gl.FLOAT_MAT2: return 2;
            case gl.FLOAT_MAT2x3: return 2;
            case gl.FLOAT_MAT2x4: return 2;
            case gl.FLOAT_MAT3x2: return 3;
            case gl.FLOAT_MAT3: return 3;
            case gl.FLOAT_MAT3x4: return 3;
            case gl.FLOAT_MAT4x2: return 4;
            case gl.FLOAT_MAT4x3: return 4;
            case gl.FLOAT_MAT4: return 4;
            default: {
                return 1;
            }
        }
    }
    function WebGL2CmdFuncCreateBuffer(device, gpuBuffer) {
        const { gl } = device;
        const cache = device.stateCache;
        const glUsage = gpuBuffer.memUsage & define_29.MemoryUsageBit.HOST ? gl.DYNAMIC_DRAW : gl.STATIC_DRAW;
        if (gpuBuffer.usage & define_29.BufferUsageBit.VERTEX) {
            gpuBuffer.glTarget = gl.ARRAY_BUFFER;
            const glBuffer = gl.createBuffer();
            if (glBuffer) {
                gpuBuffer.glBuffer = glBuffer;
                if (gpuBuffer.size > 0) {
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            gl.bindVertexArray(null);
                            cache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    if (device.stateCache.glArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ARRAY_BUFFER, gpuBuffer.glBuffer);
                        device.stateCache.glArrayBuffer = gpuBuffer.glBuffer;
                    }
                    gl.bufferData(gl.ARRAY_BUFFER, gpuBuffer.size, glUsage);
                    gl.bindBuffer(gl.ARRAY_BUFFER, null);
                    device.stateCache.glArrayBuffer = null;
                }
            }
        }
        else if (gpuBuffer.usage & define_29.BufferUsageBit.INDEX) {
            gpuBuffer.glTarget = gl.ELEMENT_ARRAY_BUFFER;
            const glBuffer = gl.createBuffer();
            if (glBuffer) {
                gpuBuffer.glBuffer = glBuffer;
                if (gpuBuffer.size > 0) {
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            gl.bindVertexArray(null);
                            cache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    if (device.stateCache.glElementArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
                        device.stateCache.glElementArrayBuffer = gpuBuffer.glBuffer;
                    }
                    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.size, glUsage);
                    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
                    device.stateCache.glElementArrayBuffer = null;
                }
            }
        }
        else if (gpuBuffer.usage & define_29.BufferUsageBit.UNIFORM) {
            gpuBuffer.glTarget = gl.UNIFORM_BUFFER;
            const glBuffer = gl.createBuffer();
            if (glBuffer && gpuBuffer.size > 0) {
                gpuBuffer.glBuffer = glBuffer;
                if (device.stateCache.glUniformBuffer !== gpuBuffer.glBuffer) {
                    gl.bindBuffer(gl.UNIFORM_BUFFER, gpuBuffer.glBuffer);
                    device.stateCache.glUniformBuffer = gpuBuffer.glBuffer;
                }
                gl.bufferData(gl.UNIFORM_BUFFER, gpuBuffer.size, glUsage);
                gl.bindBuffer(gl.UNIFORM_BUFFER, null);
                device.stateCache.glUniformBuffer = null;
            }
        }
        else if (gpuBuffer.usage & define_29.BufferUsageBit.INDIRECT) {
            gpuBuffer.glTarget = gl.NONE;
        }
        else if (gpuBuffer.usage & define_29.BufferUsageBit.TRANSFER_DST) {
            gpuBuffer.glTarget = gl.NONE;
        }
        else if (gpuBuffer.usage & define_29.BufferUsageBit.TRANSFER_SRC) {
            gpuBuffer.glTarget = gl.NONE;
        }
        else {
            console.error('Unsupported BufferType, create buffer failed.');
            gpuBuffer.glTarget = gl.NONE;
        }
    }
    exports_51("WebGL2CmdFuncCreateBuffer", WebGL2CmdFuncCreateBuffer);
    function WebGL2CmdFuncDestroyBuffer(device, gpuBuffer) {
        const { gl } = device;
        const cache = device.stateCache;
        if (gpuBuffer.glBuffer) {
            // Firefox 75+ implicitly unbind whatever buffer there was on the slot sometimes
            // can be reproduced in the static batching scene at https://github.com/cocos-creator/test-cases-3d
            switch (gpuBuffer.glTarget) {
                case gl.ARRAY_BUFFER:
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            gl.bindVertexArray(null);
                            device.stateCache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    gl.bindBuffer(gl.ARRAY_BUFFER, null);
                    device.stateCache.glArrayBuffer = null;
                    break;
                case gl.ELEMENT_ARRAY_BUFFER:
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            gl.bindVertexArray(null);
                            device.stateCache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
                    device.stateCache.glElementArrayBuffer = null;
                    break;
                case gl.UNIFORM_BUFFER:
                    gl.bindBuffer(gl.UNIFORM_BUFFER, null);
                    device.stateCache.glUniformBuffer = null;
                    break;
                default:
            }
            gl.deleteBuffer(gpuBuffer.glBuffer);
            gpuBuffer.glBuffer = null;
        }
    }
    exports_51("WebGL2CmdFuncDestroyBuffer", WebGL2CmdFuncDestroyBuffer);
    function WebGL2CmdFuncResizeBuffer(device, gpuBuffer) {
        const { gl } = device;
        const cache = device.stateCache;
        const glUsage = gpuBuffer.memUsage & define_29.MemoryUsageBit.HOST ? gl.DYNAMIC_DRAW : gl.STATIC_DRAW;
        if (gpuBuffer.usage & define_29.BufferUsageBit.VERTEX) {
            if (device.extensions.useVAO) {
                if (cache.glVAO) {
                    gl.bindVertexArray(null);
                    cache.glVAO = null;
                }
            }
            gfxStateCache.gpuInputAssembler = null;
            if (cache.glArrayBuffer !== gpuBuffer.glBuffer) {
                gl.bindBuffer(gl.ARRAY_BUFFER, gpuBuffer.glBuffer);
            }
            if (gpuBuffer.buffer) {
                gl.bufferData(gl.ARRAY_BUFFER, gpuBuffer.buffer, glUsage);
            }
            else {
                gl.bufferData(gl.ARRAY_BUFFER, gpuBuffer.size, glUsage);
            }
            gl.bindBuffer(gl.ARRAY_BUFFER, null);
            cache.glArrayBuffer = null;
        }
        else if (gpuBuffer.usage & define_29.BufferUsageBit.INDEX) {
            if (device.extensions.useVAO) {
                if (cache.glVAO) {
                    gl.bindVertexArray(null);
                    cache.glVAO = null;
                }
            }
            gfxStateCache.gpuInputAssembler = null;
            if (device.stateCache.glElementArrayBuffer !== gpuBuffer.glBuffer) {
                gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
            }
            if (gpuBuffer.buffer) {
                gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.buffer, glUsage);
            }
            else {
                gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.size, glUsage);
            }
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
            device.stateCache.glElementArrayBuffer = null;
        }
        else if (gpuBuffer.usage & define_29.BufferUsageBit.UNIFORM) {
            if (device.stateCache.glUniformBuffer !== gpuBuffer.glBuffer) {
                gl.bindBuffer(gl.UNIFORM_BUFFER, gpuBuffer.glBuffer);
            }
            gl.bufferData(gl.UNIFORM_BUFFER, gpuBuffer.size, glUsage);
            gl.bindBuffer(gl.UNIFORM_BUFFER, null);
            device.stateCache.glUniformBuffer = null;
        }
        else if ((gpuBuffer.usage & define_29.BufferUsageBit.INDIRECT)
            || (gpuBuffer.usage & define_29.BufferUsageBit.TRANSFER_DST)
            || (gpuBuffer.usage & define_29.BufferUsageBit.TRANSFER_SRC)) {
            gpuBuffer.glTarget = gl.NONE;
        }
        else {
            console.error('Unsupported BufferType, create buffer failed.');
            gpuBuffer.glTarget = gl.NONE;
        }
    }
    exports_51("WebGL2CmdFuncResizeBuffer", WebGL2CmdFuncResizeBuffer);
    function WebGL2CmdFuncUpdateBuffer(device, gpuBuffer, buffer, offset, size) {
        if (gpuBuffer.usage & define_29.BufferUsageBit.INDIRECT) {
            gpuBuffer.indirects.clearDraws();
            const drawInfos = buffer.drawInfos;
            for (let i = 0; i < drawInfos.length; ++i) {
                gpuBuffer.indirects.setDrawInfo(offset + i, drawInfos[i]);
            }
        }
        else {
            const buff = buffer;
            const { gl } = device;
            const cache = device.stateCache;
            switch (gpuBuffer.glTarget) {
                case gl.ARRAY_BUFFER: {
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            gl.bindVertexArray(null);
                            cache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    if (cache.glArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ARRAY_BUFFER, gpuBuffer.glBuffer);
                        cache.glArrayBuffer = gpuBuffer.glBuffer;
                    }
                    if (size === buff.byteLength) {
                        gl.bufferSubData(gpuBuffer.glTarget, offset, buff);
                    }
                    else {
                        gl.bufferSubData(gpuBuffer.glTarget, offset, buff.slice(0, size));
                    }
                    break;
                }
                case gl.ELEMENT_ARRAY_BUFFER: {
                    if (device.extensions.useVAO) {
                        if (cache.glVAO) {
                            gl.bindVertexArray(null);
                            cache.glVAO = null;
                        }
                    }
                    gfxStateCache.gpuInputAssembler = null;
                    if (cache.glElementArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
                        cache.glElementArrayBuffer = gpuBuffer.glBuffer;
                    }
                    if (size === buff.byteLength) {
                        gl.bufferSubData(gpuBuffer.glTarget, offset, buff);
                    }
                    else {
                        gl.bufferSubData(gpuBuffer.glTarget, offset, buff.slice(0, size));
                    }
                    break;
                }
                case gl.UNIFORM_BUFFER: {
                    if (cache.glUniformBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.UNIFORM_BUFFER, gpuBuffer.glBuffer);
                        cache.glUniformBuffer = gpuBuffer.glBuffer;
                    }
                    if (size === buff.byteLength) {
                        gl.bufferSubData(gpuBuffer.glTarget, offset, buff);
                    }
                    else {
                        gl.bufferSubData(gpuBuffer.glTarget, offset, new Float32Array(buff, 0, size / 4));
                    }
                    break;
                }
                default: {
                    console.error('Unsupported BufferType, update buffer failed.');
                }
            }
        }
    }
    exports_51("WebGL2CmdFuncUpdateBuffer", WebGL2CmdFuncUpdateBuffer);
    function WebGL2CmdFuncCreateTexture(device, gpuTexture) {
        const { gl } = device;
        gpuTexture.glInternalFmt = GFXFormatToWebGLInternalFormat(gpuTexture.format, gl);
        gpuTexture.glFormat = GFXFormatToWebGLFormat(gpuTexture.format, gl);
        gpuTexture.glType = GFXFormatToWebGLType(gpuTexture.format, gl);
        let w = gpuTexture.width;
        let h = gpuTexture.height;
        switch (gpuTexture.type) {
            case define_29.TextureType.TEX2D: {
                gpuTexture.glTarget = gl.TEXTURE_2D;
                if (gpuTexture.isSwapchainTexture)
                    break;
                const maxSize = Math.max(w, h);
                if (maxSize > device.capabilities.maxTextureSize) {
                    debug_4.errorID(9100, maxSize, device.capabilities.maxTextureSize);
                }
                if (gpuTexture.samples === define_29.SampleCount.ONE) {
                    gpuTexture.glTexture = gl.createTexture();
                    if (gpuTexture.size > 0) {
                        const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
                        if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                            gl.bindTexture(gl.TEXTURE_2D, gpuTexture.glTexture);
                            glTexUnit.glTexture = gpuTexture.glTexture;
                        }
                        if (define_29.FormatInfos[gpuTexture.format].isCompressed) {
                            for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                                const imgSize = define_29.FormatSize(gpuTexture.format, w, h, 1);
                                const view = new Uint8Array(imgSize);
                                gl.compressedTexImage2D(gl.TEXTURE_2D, i, gpuTexture.glInternalFmt, w, h, 0, view);
                                w = Math.max(1, w >> 1);
                                h = Math.max(1, h >> 1);
                            }
                        }
                        else {
                            gl.texStorage2D(gl.TEXTURE_2D, gpuTexture.mipLevel, gpuTexture.glInternalFmt, w, h);
                        }
                    }
                }
                else {
                    gpuTexture.glRenderbuffer = gl.createRenderbuffer();
                    if (gpuTexture.size > 0) {
                        if (device.stateCache.glRenderbuffer !== gpuTexture.glRenderbuffer) {
                            gl.bindRenderbuffer(gl.RENDERBUFFER, gpuTexture.glRenderbuffer);
                            device.stateCache.glRenderbuffer = gpuTexture.glRenderbuffer;
                        }
                        gl.renderbufferStorageMultisample(gl.RENDERBUFFER, gpuTexture.samples, gpuTexture.glInternalFmt, gpuTexture.width, gpuTexture.height);
                    }
                }
                break;
            }
            case define_29.TextureType.CUBE: {
                gpuTexture.glTarget = gl.TEXTURE_CUBE_MAP;
                const maxSize = Math.max(w, h);
                if (maxSize > device.capabilities.maxCubeMapTextureSize) {
                    debug_4.errorID(9100, maxSize, device.capabilities.maxTextureSize);
                }
                gpuTexture.glTexture = gl.createTexture();
                if (gpuTexture.size > 0) {
                    const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
                    if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                        gl.bindTexture(gl.TEXTURE_CUBE_MAP, gpuTexture.glTexture);
                        glTexUnit.glTexture = gpuTexture.glTexture;
                    }
                    if (define_29.FormatInfos[gpuTexture.format].isCompressed) {
                        for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                            const imgSize = define_29.FormatSize(gpuTexture.format, w, h, 1);
                            const view = new Uint8Array(imgSize);
                            for (let f = 0; f < 6; ++f) {
                                gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, i, gpuTexture.glInternalFmt, w, h, 0, view);
                            }
                            w = Math.max(1, w >> 1);
                            h = Math.max(1, h >> 1);
                        }
                    }
                    else {
                        gl.texStorage2D(gl.TEXTURE_CUBE_MAP, gpuTexture.mipLevel, gpuTexture.glInternalFmt, w, h);
                    }
                }
                break;
            }
            default: {
                console.error('Unsupported TextureType, create texture failed.');
                gpuTexture.type = define_29.TextureType.TEX2D;
                gpuTexture.glTarget = gl.TEXTURE_2D;
            }
        }
    }
    exports_51("WebGL2CmdFuncCreateTexture", WebGL2CmdFuncCreateTexture);
    function WebGL2CmdFuncDestroyTexture(device, gpuTexture) {
        if (gpuTexture.glTexture) {
            device.gl.deleteTexture(gpuTexture.glTexture);
            gpuTexture.glTexture = null;
        }
        if (gpuTexture.glRenderbuffer) {
            device.gl.deleteRenderbuffer(gpuTexture.glRenderbuffer);
            gpuTexture.glRenderbuffer = null;
        }
    }
    exports_51("WebGL2CmdFuncDestroyTexture", WebGL2CmdFuncDestroyTexture);
    function WebGL2CmdFuncResizeTexture(device, gpuTexture) {
        if (!gpuTexture.size)
            return;
        const { gl } = device;
        let w = gpuTexture.width;
        let h = gpuTexture.height;
        switch (gpuTexture.type) {
            case define_29.TextureType.TEX2D: {
                gpuTexture.glTarget = gl.TEXTURE_2D;
                const maxSize = Math.max(w, h);
                if (maxSize > device.capabilities.maxTextureSize) {
                    debug_4.errorID(9100, maxSize, device.capabilities.maxTextureSize);
                }
                if (gpuTexture.samples === define_29.SampleCount.ONE) {
                    const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
                    if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                        gl.bindTexture(gl.TEXTURE_2D, gpuTexture.glTexture);
                        glTexUnit.glTexture = gpuTexture.glTexture;
                    }
                    if (define_29.FormatInfos[gpuTexture.format].isCompressed) {
                        for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                            const imgSize = define_29.FormatSize(gpuTexture.format, w, h, 1);
                            const view = new Uint8Array(imgSize);
                            gl.compressedTexImage2D(gl.TEXTURE_2D, i, gpuTexture.glInternalFmt, w, h, 0, view);
                            w = Math.max(1, w >> 1);
                            h = Math.max(1, h >> 1);
                        }
                    }
                    else {
                        // immutable by default
                        WebGL2CmdFuncDestroyTexture(device, gpuTexture);
                        WebGL2CmdFuncCreateTexture(device, gpuTexture);
                    }
                }
                else if (gpuTexture.glRenderbuffer) {
                    if (device.stateCache.glRenderbuffer !== gpuTexture.glRenderbuffer) {
                        gl.bindRenderbuffer(gl.RENDERBUFFER, gpuTexture.glRenderbuffer);
                        device.stateCache.glRenderbuffer = gpuTexture.glRenderbuffer;
                    }
                    gl.renderbufferStorageMultisample(gl.RENDERBUFFER, gpuTexture.samples, gpuTexture.glInternalFmt, gpuTexture.width, gpuTexture.height);
                }
                break;
            }
            case define_29.TextureType.CUBE: {
                gpuTexture.type = define_29.TextureType.CUBE;
                gpuTexture.glTarget = gl.TEXTURE_CUBE_MAP;
                const maxSize = Math.max(w, h);
                if (maxSize > device.capabilities.maxCubeMapTextureSize) {
                    debug_4.errorID(9100, maxSize, device.capabilities.maxTextureSize);
                }
                const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
                if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                    gl.bindTexture(gl.TEXTURE_CUBE_MAP, gpuTexture.glTexture);
                    glTexUnit.glTexture = gpuTexture.glTexture;
                }
                if (define_29.FormatInfos[gpuTexture.format].isCompressed) {
                    for (let f = 0; f < 6; ++f) {
                        w = gpuTexture.width;
                        h = gpuTexture.height;
                        for (let i = 0; i < gpuTexture.mipLevel; ++i) {
                            const imgSize = define_29.FormatSize(gpuTexture.format, w, h, 1);
                            const view = new Uint8Array(imgSize);
                            gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, i, gpuTexture.glInternalFmt, w, h, 0, view);
                            w = Math.max(1, w >> 1);
                            h = Math.max(1, h >> 1);
                        }
                    }
                }
                else {
                    // immutable by default
                    WebGL2CmdFuncDestroyTexture(device, gpuTexture);
                    WebGL2CmdFuncCreateTexture(device, gpuTexture);
                }
                break;
            }
            default: {
                console.error('Unsupported TextureType, create texture failed.');
                gpuTexture.type = define_29.TextureType.TEX2D;
                gpuTexture.glTarget = gl.TEXTURE_2D;
            }
        }
    }
    exports_51("WebGL2CmdFuncResizeTexture", WebGL2CmdFuncResizeTexture);
    function WebGL2CmdFuncCreateSampler(device, gpuSampler) {
        const { gl } = device;
        const glSampler = gl.createSampler();
        if (glSampler) {
            if (gpuSampler.minFilter === define_29.Filter.LINEAR || gpuSampler.minFilter === define_29.Filter.ANISOTROPIC) {
                if (gpuSampler.mipFilter === define_29.Filter.LINEAR || gpuSampler.mipFilter === define_29.Filter.ANISOTROPIC) {
                    gpuSampler.glMinFilter = gl.LINEAR_MIPMAP_LINEAR;
                }
                else if (gpuSampler.mipFilter === define_29.Filter.POINT) {
                    gpuSampler.glMinFilter = gl.LINEAR_MIPMAP_NEAREST;
                }
                else {
                    gpuSampler.glMinFilter = gl.LINEAR;
                }
            }
            else if (gpuSampler.mipFilter === define_29.Filter.LINEAR || gpuSampler.mipFilter === define_29.Filter.ANISOTROPIC) {
                gpuSampler.glMinFilter = gl.NEAREST_MIPMAP_LINEAR;
            }
            else if (gpuSampler.mipFilter === define_29.Filter.POINT) {
                gpuSampler.glMinFilter = gl.NEAREST_MIPMAP_NEAREST;
            }
            else {
                gpuSampler.glMinFilter = gl.NEAREST;
            }
            if (gpuSampler.magFilter === define_29.Filter.LINEAR || gpuSampler.magFilter === define_29.Filter.ANISOTROPIC) {
                gpuSampler.glMagFilter = gl.LINEAR;
            }
            else {
                gpuSampler.glMagFilter = gl.NEAREST;
            }
            gpuSampler.glWrapS = WebGLWraps[gpuSampler.addressU];
            gpuSampler.glWrapT = WebGLWraps[gpuSampler.addressV];
            gpuSampler.glWrapR = WebGLWraps[gpuSampler.addressW];
            gpuSampler.glSampler = glSampler;
            gl.samplerParameteri(glSampler, gl.TEXTURE_MIN_FILTER, gpuSampler.glMinFilter);
            gl.samplerParameteri(glSampler, gl.TEXTURE_MAG_FILTER, gpuSampler.glMagFilter);
            gl.samplerParameteri(glSampler, gl.TEXTURE_WRAP_S, gpuSampler.glWrapS);
            gl.samplerParameteri(glSampler, gl.TEXTURE_WRAP_T, gpuSampler.glWrapT);
            gl.samplerParameteri(glSampler, gl.TEXTURE_WRAP_R, gpuSampler.glWrapR);
            gl.samplerParameterf(glSampler, gl.TEXTURE_MIN_LOD, 0);
            gl.samplerParameterf(glSampler, gl.TEXTURE_MAX_LOD, 1000);
        }
    }
    exports_51("WebGL2CmdFuncCreateSampler", WebGL2CmdFuncCreateSampler);
    function WebGL2CmdFuncDestroySampler(device, gpuSampler) {
        if (gpuSampler.glSampler) {
            device.gl.deleteSampler(gpuSampler.glSampler);
            gpuSampler.glSampler = null;
        }
    }
    exports_51("WebGL2CmdFuncDestroySampler", WebGL2CmdFuncDestroySampler);
    function WebGL2CmdFuncCreateFramebuffer(device, gpuFramebuffer) {
        for (let i = 0; i < gpuFramebuffer.gpuColorTextures.length; ++i) {
            const tex = gpuFramebuffer.gpuColorTextures[i];
            if (tex.isSwapchainTexture) {
                gpuFramebuffer.isOffscreen = false;
                return;
            }
        }
        const { gl } = device;
        const attachments = [];
        const glFramebuffer = gl.createFramebuffer();
        if (glFramebuffer) {
            gpuFramebuffer.glFramebuffer = glFramebuffer;
            if (device.stateCache.glFramebuffer !== gpuFramebuffer.glFramebuffer) {
                gl.bindFramebuffer(gl.FRAMEBUFFER, gpuFramebuffer.glFramebuffer);
            }
            for (let i = 0; i < gpuFramebuffer.gpuColorTextures.length; ++i) {
                const colorTexture = gpuFramebuffer.gpuColorTextures[i];
                if (colorTexture) {
                    if (colorTexture.glTexture) {
                        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0 + i, colorTexture.glTarget, colorTexture.glTexture, 0); // level should be 0.
                    }
                    else {
                        gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0 + i, gl.RENDERBUFFER, colorTexture.glRenderbuffer);
                    }
                    attachments.push(gl.COLOR_ATTACHMENT0 + i);
                    gpuFramebuffer.width = Math.min(gpuFramebuffer.width, colorTexture.width);
                    gpuFramebuffer.height = Math.min(gpuFramebuffer.height, colorTexture.height);
                }
            }
            const dst = gpuFramebuffer.gpuDepthStencilTexture;
            if (dst) {
                const glAttachment = define_29.FormatInfos[dst.format].hasStencil ? gl.DEPTH_STENCIL_ATTACHMENT : gl.DEPTH_ATTACHMENT;
                if (dst.glTexture) {
                    gl.framebufferTexture2D(gl.FRAMEBUFFER, glAttachment, dst.glTarget, dst.glTexture, 0); // level must be 0
                }
                else {
                    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, glAttachment, gl.RENDERBUFFER, dst.glRenderbuffer);
                }
                gpuFramebuffer.width = Math.min(gpuFramebuffer.width, dst.width);
                gpuFramebuffer.height = Math.min(gpuFramebuffer.height, dst.height);
            }
            gl.drawBuffers(attachments);
            const status = gl.checkFramebufferStatus(gl.FRAMEBUFFER);
            if (status !== gl.FRAMEBUFFER_COMPLETE) {
                switch (status) {
                    case gl.FRAMEBUFFER_INCOMPLETE_ATTACHMENT: {
                        console.error('glCheckFramebufferStatus() - FRAMEBUFFER_INCOMPLETE_ATTACHMENT');
                        break;
                    }
                    case gl.FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: {
                        console.error('glCheckFramebufferStatus() - FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT');
                        break;
                    }
                    case gl.FRAMEBUFFER_INCOMPLETE_DIMENSIONS: {
                        console.error('glCheckFramebufferStatus() - FRAMEBUFFER_INCOMPLETE_DIMENSIONS');
                        break;
                    }
                    case gl.FRAMEBUFFER_UNSUPPORTED: {
                        console.error('glCheckFramebufferStatus() - FRAMEBUFFER_UNSUPPORTED');
                        break;
                    }
                    default:
                }
            }
            if (device.stateCache.glFramebuffer !== gpuFramebuffer.glFramebuffer) {
                gl.bindFramebuffer(gl.FRAMEBUFFER, device.stateCache.glFramebuffer);
            }
        }
    }
    exports_51("WebGL2CmdFuncCreateFramebuffer", WebGL2CmdFuncCreateFramebuffer);
    function WebGL2CmdFuncDestroyFramebuffer(device, gpuFramebuffer) {
        if (gpuFramebuffer.glFramebuffer) {
            device.gl.deleteFramebuffer(gpuFramebuffer.glFramebuffer);
            gpuFramebuffer.glFramebuffer = null;
        }
    }
    exports_51("WebGL2CmdFuncDestroyFramebuffer", WebGL2CmdFuncDestroyFramebuffer);
    function WebGL2CmdFuncCreateShader(device, gpuShader) {
        const { gl } = device;
        for (let k = 0; k < gpuShader.gpuStages.length; k++) {
            const gpuStage = gpuShader.gpuStages[k];
            let glShaderType = 0;
            let shaderTypeStr = '';
            let lineNumber = 1;
            switch (gpuStage.type) {
                case define_29.ShaderStageFlagBit.VERTEX: {
                    shaderTypeStr = 'VertexShader';
                    glShaderType = gl.VERTEX_SHADER;
                    break;
                }
                case define_29.ShaderStageFlagBit.FRAGMENT: {
                    shaderTypeStr = 'FragmentShader';
                    glShaderType = gl.FRAGMENT_SHADER;
                    break;
                }
                default: {
                    console.error('Unsupported ShaderType.');
                    return;
                }
            }
            const glShader = gl.createShader(glShaderType);
            if (glShader) {
                gpuStage.glShader = glShader;
                gl.shaderSource(gpuStage.glShader, `#version 300 es\n${gpuStage.source}`);
                gl.compileShader(gpuStage.glShader);
                if (!gl.getShaderParameter(gpuStage.glShader, gl.COMPILE_STATUS)) {
                    console.error(`${shaderTypeStr} in '${gpuShader.name}' compilation failed.`);
                    console.error('Shader source dump:', gpuStage.source.replace(/^|\n/g, () => `\n${lineNumber++} `));
                    console.error(gl.getShaderInfoLog(gpuStage.glShader));
                    for (let l = 0; l < gpuShader.gpuStages.length; l++) {
                        const stage = gpuShader.gpuStages[k];
                        if (stage.glShader) {
                            gl.deleteShader(stage.glShader);
                            stage.glShader = null;
                        }
                    }
                    return;
                }
            }
        }
        const glProgram = gl.createProgram();
        if (!glProgram) {
            return;
        }
        gpuShader.glProgram = glProgram;
        // link program
        for (let k = 0; k < gpuShader.gpuStages.length; k++) {
            const gpuStage = gpuShader.gpuStages[k];
            gl.attachShader(gpuShader.glProgram, gpuStage.glShader);
        }
        gl.linkProgram(gpuShader.glProgram);
        // detach & delete immediately
        for (let k = 0; k < gpuShader.gpuStages.length; k++) {
            const gpuStage = gpuShader.gpuStages[k];
            if (gpuStage.glShader) {
                gl.detachShader(gpuShader.glProgram, gpuStage.glShader);
                gl.deleteShader(gpuStage.glShader);
                gpuStage.glShader = null;
            }
        }
        if (gl.getProgramParameter(gpuShader.glProgram, gl.LINK_STATUS)) {
            debug_4.debug(`Shader '${gpuShader.name}' compilation succeeded.`);
        }
        else {
            console.error(`Failed to link shader '${gpuShader.name}'.`);
            console.error(gl.getProgramInfoLog(gpuShader.glProgram));
            return;
        }
        // parse inputs
        const activeAttribCount = gl.getProgramParameter(gpuShader.glProgram, gl.ACTIVE_ATTRIBUTES);
        gpuShader.glInputs = new Array(activeAttribCount);
        for (let i = 0; i < activeAttribCount; ++i) {
            const attribInfo = gl.getActiveAttrib(gpuShader.glProgram, i);
            if (attribInfo) {
                let varName;
                const nameOffset = attribInfo.name.indexOf('[');
                if (nameOffset !== -1) {
                    varName = attribInfo.name.substr(0, nameOffset);
                }
                else {
                    varName = attribInfo.name;
                }
                const glLoc = gl.getAttribLocation(gpuShader.glProgram, varName);
                const type = WebGLTypeToGFXType(attribInfo.type, gl);
                const stride = WebGLGetTypeSize(attribInfo.type, gl);
                gpuShader.glInputs[i] = {
                    name: varName,
                    type,
                    stride,
                    count: attribInfo.size,
                    size: stride * attribInfo.size,
                    glType: attribInfo.type,
                    glLoc,
                };
            }
        }
        // create uniform blocks
        const activeBlockCount = gl.getProgramParameter(gpuShader.glProgram, gl.ACTIVE_UNIFORM_BLOCKS);
        let blockName;
        let blockIdx;
        let blockSize;
        let block;
        if (activeBlockCount) {
            gpuShader.glBlocks = new Array(activeBlockCount);
            for (let b = 0; b < activeBlockCount; ++b) {
                blockName = gl.getActiveUniformBlockName(gpuShader.glProgram, b);
                const nameOffset = blockName.indexOf('[');
                if (nameOffset !== -1) {
                    blockName = blockName.substr(0, nameOffset);
                }
                // blockIdx = gl.getUniformBlockIndex(gpuShader.glProgram, blockName);
                block = null;
                for (let k = 0; k < gpuShader.blocks.length; k++) {
                    if (gpuShader.blocks[k].name === blockName) {
                        block = gpuShader.blocks[k];
                        break;
                    }
                }
                if (!block) {
                    debug_4.error(`Block '${blockName}' does not bound`);
                }
                else {
                    // blockIdx = gl.getUniformBlockIndex(gpuShader.glProgram, blockName);
                    blockIdx = b;
                    blockSize = gl.getActiveUniformBlockParameter(gpuShader.glProgram, blockIdx, gl.UNIFORM_BLOCK_DATA_SIZE);
                    const glBinding = block.binding + (device.bindingMappingInfo.bufferOffsets[block.set] || 0);
                    gl.uniformBlockBinding(gpuShader.glProgram, blockIdx, glBinding);
                    gpuShader.glBlocks[b] = {
                        set: block.set,
                        binding: block.binding,
                        idx: blockIdx,
                        name: blockName,
                        size: blockSize,
                        glBinding,
                    };
                }
            }
        }
        // WebGL doesn't support Framebuffer Fetch
        for (let i = 0; i < gpuShader.subpassInputs.length; ++i) {
            const subpassInput = gpuShader.subpassInputs[i];
            gpuShader.samplerTextures.push(new define_29.UniformSamplerTexture(subpassInput.set, subpassInput.binding, subpassInput.name, define_29.Type.SAMPLER2D, subpassInput.count));
        }
        // create uniform sampler textures
        if (gpuShader.samplerTextures.length > 0) {
            gpuShader.glSamplerTextures = new Array(gpuShader.samplerTextures.length);
            for (let i = 0; i < gpuShader.samplerTextures.length; ++i) {
                const sampler = gpuShader.samplerTextures[i];
                gpuShader.glSamplerTextures[i] = {
                    set: sampler.set,
                    binding: sampler.binding,
                    name: sampler.name,
                    type: sampler.type,
                    count: sampler.count,
                    units: [],
                    glUnits: null,
                    glType: GFXTypeToWebGLType(sampler.type, gl),
                    glLoc: null,
                };
            }
        }
        // texture unit index mapping optimization
        const glActiveSamplers = [];
        const glActiveSamplerLocations = [];
        const texUnitCacheMap = device.stateCache.texUnitCacheMap;
        let flexibleSetBaseOffset = 0;
        for (let i = 0; i < gpuShader.blocks.length; ++i) {
            if (gpuShader.blocks[i].set === device.bindingMappingInfo.flexibleSet) {
                flexibleSetBaseOffset++;
            }
        }
        let arrayOffset = 0;
        for (let i = 0; i < gpuShader.samplerTextures.length; ++i) {
            const sampler = gpuShader.samplerTextures[i];
            const glLoc = gl.getUniformLocation(gpuShader.glProgram, sampler.name);
            // wEcHAT just returns { id: -1 } for non-existing names /eyerolling
            if (glLoc && glLoc.id !== -1) {
                glActiveSamplers.push(gpuShader.glSamplerTextures[i]);
                glActiveSamplerLocations.push(glLoc);
            }
            if (texUnitCacheMap[sampler.name] === undefined) {
                let binding = sampler.binding + device.bindingMappingInfo.samplerOffsets[sampler.set] + arrayOffset;
                if (sampler.set === device.bindingMappingInfo.flexibleSet) {
                    binding -= flexibleSetBaseOffset;
                }
                texUnitCacheMap[sampler.name] = binding % device.capabilities.maxTextureUnits;
                arrayOffset += sampler.count - 1;
            }
        }
        if (glActiveSamplers.length) {
            const usedTexUnits = [];
            // try to reuse existing mappings first
            for (let i = 0; i < glActiveSamplers.length; ++i) {
                const glSampler = glActiveSamplers[i];
                let cachedUnit = texUnitCacheMap[glSampler.name];
                if (cachedUnit !== undefined) {
                    glSampler.glLoc = glActiveSamplerLocations[i];
                    for (let t = 0; t < glSampler.count; ++t) {
                        while (usedTexUnits[cachedUnit]) {
                            cachedUnit = (cachedUnit + 1) % device.capabilities.maxTextureUnits;
                        }
                        glSampler.units.push(cachedUnit);
                        usedTexUnits[cachedUnit] = true;
                    }
                }
            }
            // fill in the rest sequencially
            let unitIdx = 0;
            for (let i = 0; i < glActiveSamplers.length; ++i) {
                const glSampler = glActiveSamplers[i];
                if (!glSampler.glLoc) {
                    glSampler.glLoc = glActiveSamplerLocations[i];
                    while (usedTexUnits[unitIdx]) {
                        unitIdx++;
                    }
                    for (let t = 0; t < glSampler.count; ++t) {
                        while (usedTexUnits[unitIdx]) {
                            unitIdx = (unitIdx + 1) % device.capabilities.maxTextureUnits;
                        }
                        if (texUnitCacheMap[glSampler.name] === undefined) {
                            texUnitCacheMap[glSampler.name] = unitIdx;
                        }
                        glSampler.units.push(unitIdx);
                        usedTexUnits[unitIdx] = true;
                    }
                }
            }
            if (device.stateCache.glProgram !== gpuShader.glProgram) {
                gl.useProgram(gpuShader.glProgram);
            }
            for (let k = 0; k < glActiveSamplers.length; k++) {
                const glSampler = glActiveSamplers[k];
                glSampler.glUnits = new Int32Array(glSampler.units);
                gl.uniform1iv(glSampler.glLoc, glSampler.glUnits);
            }
            if (device.stateCache.glProgram !== gpuShader.glProgram) {
                gl.useProgram(device.stateCache.glProgram);
            }
        }
        gpuShader.glSamplerTextures = glActiveSamplers;
    }
    exports_51("WebGL2CmdFuncCreateShader", WebGL2CmdFuncCreateShader);
    function WebGL2CmdFuncDestroyShader(device, gpuShader) {
        if (gpuShader.glProgram) {
            device.gl.deleteProgram(gpuShader.glProgram);
            gpuShader.glProgram = null;
        }
    }
    exports_51("WebGL2CmdFuncDestroyShader", WebGL2CmdFuncDestroyShader);
    function WebGL2CmdFuncCreateInputAssember(device, gpuInputAssembler) {
        const { gl } = device;
        gpuInputAssembler.glAttribs = new Array(gpuInputAssembler.attributes.length);
        const offsets = [0, 0, 0, 0, 0, 0, 0, 0];
        for (let i = 0; i < gpuInputAssembler.attributes.length; ++i) {
            const attrib = gpuInputAssembler.attributes[i];
            const stream = attrib.stream !== undefined ? attrib.stream : 0;
            // if (stream < gpuInputAssembler.gpuVertexBuffers.length) {
            const gpuBuffer = gpuInputAssembler.gpuVertexBuffers[stream];
            const glType = GFXFormatToWebGLType(attrib.format, gl);
            const { size } = define_29.FormatInfos[attrib.format];
            gpuInputAssembler.glAttribs[i] = {
                name: attrib.name,
                glBuffer: gpuBuffer.glBuffer,
                glType,
                size,
                count: define_29.FormatInfos[attrib.format].count,
                stride: gpuBuffer.stride,
                componentCount: WebGLGetComponentCount(glType, gl),
                isNormalized: (attrib.isNormalized !== undefined ? attrib.isNormalized : false),
                isInstanced: (attrib.isInstanced !== undefined ? attrib.isInstanced : false),
                offset: offsets[stream],
            };
            offsets[stream] += size;
        }
    }
    exports_51("WebGL2CmdFuncCreateInputAssember", WebGL2CmdFuncCreateInputAssember);
    function WebGL2CmdFuncDestroyInputAssembler(device, gpuInputAssembler) {
        const it = gpuInputAssembler.glVAOs.values();
        let res = it.next();
        while (!res.done) {
            device.gl.deleteVertexArray(res.value);
            res = it.next();
        }
        gpuInputAssembler.glVAOs.clear();
    }
    exports_51("WebGL2CmdFuncDestroyInputAssembler", WebGL2CmdFuncDestroyInputAssembler);
    function WebGL2CmdFuncBeginRenderPass(device, gpuRenderPass, gpuFramebuffer, renderArea, clearColors, clearDepth, clearStencil) {
        const { gl } = device;
        const cache = device.stateCache;
        let clears = 0;
        if (gpuFramebuffer && gpuRenderPass) {
            if (cache.glFramebuffer !== gpuFramebuffer.glFramebuffer) {
                gl.bindFramebuffer(gl.FRAMEBUFFER, gpuFramebuffer.glFramebuffer);
                cache.glFramebuffer = gpuFramebuffer.glFramebuffer;
            }
            if (cache.viewport.left !== renderArea.x
                || cache.viewport.top !== renderArea.y
                || cache.viewport.width !== renderArea.width
                || cache.viewport.height !== renderArea.height) {
                gl.viewport(renderArea.x, renderArea.y, renderArea.width, renderArea.height);
                cache.viewport.left = renderArea.x;
                cache.viewport.top = renderArea.y;
                cache.viewport.width = renderArea.width;
                cache.viewport.height = renderArea.height;
            }
            if (cache.scissorRect.x !== 0
                || cache.scissorRect.y !== 0
                || cache.scissorRect.width !== gpuFramebuffer.width
                || cache.scissorRect.height !== gpuFramebuffer.height) {
                gl.scissor(0, 0, gpuFramebuffer.width, gpuFramebuffer.height);
                cache.scissorRect.x = 0;
                cache.scissorRect.y = 0;
                cache.scissorRect.width = gpuFramebuffer.width;
                cache.scissorRect.height = gpuFramebuffer.height;
            }
            gfxStateCache.invalidateAttachments.length = 0;
            for (let j = 0; j < clearColors.length; ++j) {
                const colorAttachment = gpuRenderPass.colorAttachments[j];
                if (colorAttachment.format !== define_29.Format.UNKNOWN) {
                    switch (colorAttachment.loadOp) {
                        case define_29.LoadOp.LOAD: break; // GL default behavior
                        case define_29.LoadOp.CLEAR: {
                            if (cache.bs.targets[0].blendColorMask !== define_29.ColorMask.ALL) {
                                gl.colorMask(true, true, true, true);
                            }
                            if (!gpuFramebuffer.isOffscreen) {
                                const clearColor = clearColors[0];
                                gl.clearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
                                clears |= gl.COLOR_BUFFER_BIT;
                            }
                            else {
                                _f32v4[0] = clearColors[j].x;
                                _f32v4[1] = clearColors[j].y;
                                _f32v4[2] = clearColors[j].z;
                                _f32v4[3] = clearColors[j].w;
                                gl.clearBufferfv(gl.COLOR, j, _f32v4);
                            }
                            break;
                        }
                        case define_29.LoadOp.DISCARD: {
                            // invalidate the framebuffer
                            gfxStateCache.invalidateAttachments.push(gl.COLOR_ATTACHMENT0 + j);
                            break;
                        }
                        default:
                    }
                }
            } // if (curGPURenderPass)
            if (gpuRenderPass.depthStencilAttachment) {
                if (gpuRenderPass.depthStencilAttachment.format !== define_29.Format.UNKNOWN) {
                    switch (gpuRenderPass.depthStencilAttachment.depthLoadOp) {
                        case define_29.LoadOp.LOAD: break; // GL default behavior
                        case define_29.LoadOp.CLEAR: {
                            if (!cache.dss.depthWrite) {
                                gl.depthMask(true);
                            }
                            gl.clearDepth(clearDepth);
                            clears |= gl.DEPTH_BUFFER_BIT;
                            break;
                        }
                        case define_29.LoadOp.DISCARD: {
                            // invalidate the framebuffer
                            gfxStateCache.invalidateAttachments.push(gl.DEPTH_ATTACHMENT);
                            break;
                        }
                        default:
                    }
                    if (define_29.FormatInfos[gpuRenderPass.depthStencilAttachment.format].hasStencil) {
                        switch (gpuRenderPass.depthStencilAttachment.stencilLoadOp) {
                            case define_29.LoadOp.LOAD: break; // GL default behavior
                            case define_29.LoadOp.CLEAR: {
                                if (!cache.dss.stencilWriteMaskFront) {
                                    gl.stencilMaskSeparate(gl.FRONT, 0xffff);
                                }
                                if (!cache.dss.stencilWriteMaskBack) {
                                    gl.stencilMaskSeparate(gl.BACK, 0xffff);
                                }
                                gl.clearStencil(clearStencil);
                                clears |= gl.STENCIL_BUFFER_BIT;
                                break;
                            }
                            case define_29.LoadOp.DISCARD: {
                                // invalidate the framebuffer
                                gfxStateCache.invalidateAttachments.push(gl.STENCIL_ATTACHMENT);
                                break;
                            }
                            default:
                        }
                    }
                }
            } // if (curGPURenderPass.depthStencilAttachment)
            if (gpuFramebuffer.glFramebuffer && gfxStateCache.invalidateAttachments.length) {
                gl.invalidateFramebuffer(gl.FRAMEBUFFER, gfxStateCache.invalidateAttachments);
            }
            if (clears) {
                gl.clear(clears);
            }
            // restore states
            if (clears & gl.COLOR_BUFFER_BIT) {
                const colorMask = cache.bs.targets[0].blendColorMask;
                if (colorMask !== define_29.ColorMask.ALL) {
                    const r = (colorMask & define_29.ColorMask.R) !== define_29.ColorMask.NONE;
                    const g = (colorMask & define_29.ColorMask.G) !== define_29.ColorMask.NONE;
                    const b = (colorMask & define_29.ColorMask.B) !== define_29.ColorMask.NONE;
                    const a = (colorMask & define_29.ColorMask.A) !== define_29.ColorMask.NONE;
                    gl.colorMask(r, g, b, a);
                }
            }
            if ((clears & gl.DEPTH_BUFFER_BIT)
                && !cache.dss.depthWrite) {
                gl.depthMask(false);
            }
            if (clears & gl.STENCIL_BUFFER_BIT) {
                if (!cache.dss.stencilWriteMaskFront) {
                    gl.stencilMaskSeparate(gl.FRONT, 0);
                }
                if (!cache.dss.stencilWriteMaskBack) {
                    gl.stencilMaskSeparate(gl.BACK, 0);
                }
            }
        } // if (gpuFramebuffer)
    }
    exports_51("WebGL2CmdFuncBeginRenderPass", WebGL2CmdFuncBeginRenderPass);
    function WebGL2CmdFuncBindStates(device, gpuPipelineState, gpuInputAssembler, gpuDescriptorSets, dynamicOffsets, dynamicStates) {
        const { gl } = device;
        const cache = device.stateCache;
        const gpuShader = gpuPipelineState && gpuPipelineState.gpuShader;
        let isShaderChanged = false;
        // bind pipeline
        if (gpuPipelineState && gfxStateCache.gpuPipelineState !== gpuPipelineState) {
            gfxStateCache.gpuPipelineState = gpuPipelineState;
            gfxStateCache.glPrimitive = gpuPipelineState.glPrimitive;
            if (gpuShader) {
                const { glProgram } = gpuShader;
                if (cache.glProgram !== glProgram) {
                    gl.useProgram(glProgram);
                    cache.glProgram = glProgram;
                    isShaderChanged = true;
                }
            }
            // rasterizer state
            const { rs } = gpuPipelineState;
            if (rs) {
                if (cache.rs.cullMode !== rs.cullMode) {
                    switch (rs.cullMode) {
                        case define_29.CullMode.NONE: {
                            gl.disable(gl.CULL_FACE);
                            break;
                        }
                        case define_29.CullMode.FRONT: {
                            gl.enable(gl.CULL_FACE);
                            gl.cullFace(gl.FRONT);
                            break;
                        }
                        case define_29.CullMode.BACK: {
                            gl.enable(gl.CULL_FACE);
                            gl.cullFace(gl.BACK);
                            break;
                        }
                        default:
                    }
                    device.stateCache.rs.cullMode = rs.cullMode;
                }
                const isFrontFaceCCW = rs.isFrontFaceCCW; // boolean XOR
                if (device.stateCache.rs.isFrontFaceCCW !== isFrontFaceCCW) {
                    gl.frontFace(isFrontFaceCCW ? gl.CCW : gl.CW);
                    device.stateCache.rs.isFrontFaceCCW = isFrontFaceCCW;
                }
                if ((device.stateCache.rs.depthBias !== rs.depthBias)
                    || (device.stateCache.rs.depthBiasSlop !== rs.depthBiasSlop)) {
                    gl.polygonOffset(rs.depthBias, rs.depthBiasSlop);
                    device.stateCache.rs.depthBias = rs.depthBias;
                    device.stateCache.rs.depthBiasSlop = rs.depthBiasSlop;
                }
                if (device.stateCache.rs.lineWidth !== rs.lineWidth) {
                    gl.lineWidth(rs.lineWidth);
                    device.stateCache.rs.lineWidth = rs.lineWidth;
                }
            } // rasterizater state
            // depth-stencil state
            const { dss } = gpuPipelineState;
            if (dss) {
                if (cache.dss.depthTest !== dss.depthTest) {
                    if (dss.depthTest) {
                        gl.enable(gl.DEPTH_TEST);
                    }
                    else {
                        gl.disable(gl.DEPTH_TEST);
                    }
                    cache.dss.depthTest = dss.depthTest;
                }
                if (cache.dss.depthWrite !== dss.depthWrite) {
                    gl.depthMask(dss.depthWrite);
                    cache.dss.depthWrite = dss.depthWrite;
                }
                if (cache.dss.depthFunc !== dss.depthFunc) {
                    gl.depthFunc(WebGLCmpFuncs[dss.depthFunc]);
                    cache.dss.depthFunc = dss.depthFunc;
                }
                // front
                if ((cache.dss.stencilTestFront !== dss.stencilTestFront)
                    || (cache.dss.stencilTestBack !== dss.stencilTestBack)) {
                    if (dss.stencilTestFront || dss.stencilTestBack) {
                        gl.enable(gl.STENCIL_TEST);
                    }
                    else {
                        gl.disable(gl.STENCIL_TEST);
                    }
                    cache.dss.stencilTestFront = dss.stencilTestFront;
                    cache.dss.stencilTestBack = dss.stencilTestBack;
                }
                if ((cache.dss.stencilFuncFront !== dss.stencilFuncFront)
                    || (cache.dss.stencilRefFront !== dss.stencilRefFront)
                    || (cache.dss.stencilReadMaskFront !== dss.stencilReadMaskFront)) {
                    gl.stencilFuncSeparate(gl.FRONT, WebGLCmpFuncs[dss.stencilFuncFront], dss.stencilRefFront, dss.stencilReadMaskFront);
                    cache.dss.stencilFuncFront = dss.stencilFuncFront;
                    cache.dss.stencilRefFront = dss.stencilRefFront;
                    cache.dss.stencilReadMaskFront = dss.stencilReadMaskFront;
                }
                if ((cache.dss.stencilFailOpFront !== dss.stencilFailOpFront)
                    || (cache.dss.stencilZFailOpFront !== dss.stencilZFailOpFront)
                    || (cache.dss.stencilPassOpFront !== dss.stencilPassOpFront)) {
                    gl.stencilOpSeparate(gl.FRONT, WebGLStencilOps[dss.stencilFailOpFront], WebGLStencilOps[dss.stencilZFailOpFront], WebGLStencilOps[dss.stencilPassOpFront]);
                    cache.dss.stencilFailOpFront = dss.stencilFailOpFront;
                    cache.dss.stencilZFailOpFront = dss.stencilZFailOpFront;
                    cache.dss.stencilPassOpFront = dss.stencilPassOpFront;
                }
                if (cache.dss.stencilWriteMaskFront !== dss.stencilWriteMaskFront) {
                    gl.stencilMaskSeparate(gl.FRONT, dss.stencilWriteMaskFront);
                    cache.dss.stencilWriteMaskFront = dss.stencilWriteMaskFront;
                }
                // back
                if ((cache.dss.stencilFuncBack !== dss.stencilFuncBack)
                    || (cache.dss.stencilRefBack !== dss.stencilRefBack)
                    || (cache.dss.stencilReadMaskBack !== dss.stencilReadMaskBack)) {
                    gl.stencilFuncSeparate(gl.BACK, WebGLCmpFuncs[dss.stencilFuncBack], dss.stencilRefBack, dss.stencilReadMaskBack);
                    cache.dss.stencilFuncBack = dss.stencilFuncBack;
                    cache.dss.stencilRefBack = dss.stencilRefBack;
                    cache.dss.stencilReadMaskBack = dss.stencilReadMaskBack;
                }
                if ((cache.dss.stencilFailOpBack !== dss.stencilFailOpBack)
                    || (cache.dss.stencilZFailOpBack !== dss.stencilZFailOpBack)
                    || (cache.dss.stencilPassOpBack !== dss.stencilPassOpBack)) {
                    gl.stencilOpSeparate(gl.BACK, WebGLStencilOps[dss.stencilFailOpBack], WebGLStencilOps[dss.stencilZFailOpBack], WebGLStencilOps[dss.stencilPassOpBack]);
                    cache.dss.stencilFailOpBack = dss.stencilFailOpBack;
                    cache.dss.stencilZFailOpBack = dss.stencilZFailOpBack;
                    cache.dss.stencilPassOpBack = dss.stencilPassOpBack;
                }
                if (cache.dss.stencilWriteMaskBack !== dss.stencilWriteMaskBack) {
                    gl.stencilMaskSeparate(gl.BACK, dss.stencilWriteMaskBack);
                    cache.dss.stencilWriteMaskBack = dss.stencilWriteMaskBack;
                }
            } // depth-stencil state
            // blend state
            const { bs } = gpuPipelineState;
            if (bs) {
                if (cache.bs.isA2C !== bs.isA2C) {
                    if (bs.isA2C) {
                        gl.enable(gl.SAMPLE_ALPHA_TO_COVERAGE);
                    }
                    else {
                        gl.disable(gl.SAMPLE_ALPHA_TO_COVERAGE);
                    }
                    cache.bs.isA2C = bs.isA2C;
                }
                if ((cache.bs.blendColor.x !== bs.blendColor.x)
                    || (cache.bs.blendColor.y !== bs.blendColor.y)
                    || (cache.bs.blendColor.z !== bs.blendColor.z)
                    || (cache.bs.blendColor.w !== bs.blendColor.w)) {
                    gl.blendColor(bs.blendColor.x, bs.blendColor.y, bs.blendColor.z, bs.blendColor.w);
                    cache.bs.blendColor.x = bs.blendColor.x;
                    cache.bs.blendColor.y = bs.blendColor.y;
                    cache.bs.blendColor.z = bs.blendColor.z;
                    cache.bs.blendColor.w = bs.blendColor.w;
                }
                const target0 = bs.targets[0];
                const target0Cache = cache.bs.targets[0];
                if (target0Cache.blend !== target0.blend) {
                    if (target0.blend) {
                        gl.enable(gl.BLEND);
                    }
                    else {
                        gl.disable(gl.BLEND);
                    }
                    target0Cache.blend = target0.blend;
                }
                if ((target0Cache.blendEq !== target0.blendEq)
                    || (target0Cache.blendAlphaEq !== target0.blendAlphaEq)) {
                    gl.blendEquationSeparate(WebGLBlendOps[target0.blendEq], WebGLBlendOps[target0.blendAlphaEq]);
                    target0Cache.blendEq = target0.blendEq;
                    target0Cache.blendAlphaEq = target0.blendAlphaEq;
                }
                if ((target0Cache.blendSrc !== target0.blendSrc)
                    || (target0Cache.blendDst !== target0.blendDst)
                    || (target0Cache.blendSrcAlpha !== target0.blendSrcAlpha)
                    || (target0Cache.blendDstAlpha !== target0.blendDstAlpha)) {
                    gl.blendFuncSeparate(WebGLBlendFactors[target0.blendSrc], WebGLBlendFactors[target0.blendDst], WebGLBlendFactors[target0.blendSrcAlpha], WebGLBlendFactors[target0.blendDstAlpha]);
                    target0Cache.blendSrc = target0.blendSrc;
                    target0Cache.blendDst = target0.blendDst;
                    target0Cache.blendSrcAlpha = target0.blendSrcAlpha;
                    target0Cache.blendDstAlpha = target0.blendDstAlpha;
                }
                if (target0Cache.blendColorMask !== target0.blendColorMask) {
                    gl.colorMask((target0.blendColorMask & define_29.ColorMask.R) !== define_29.ColorMask.NONE, (target0.blendColorMask & define_29.ColorMask.G) !== define_29.ColorMask.NONE, (target0.blendColorMask & define_29.ColorMask.B) !== define_29.ColorMask.NONE, (target0.blendColorMask & define_29.ColorMask.A) !== define_29.ColorMask.NONE);
                    target0Cache.blendColorMask = target0.blendColorMask;
                }
            } // blend state
        } // bind pipeline
        // bind descriptor sets
        if (gpuPipelineState && gpuPipelineState.gpuPipelineLayout && gpuShader) {
            const blockLen = gpuShader.glBlocks.length;
            const { dynamicOffsetIndices } = gpuPipelineState.gpuPipelineLayout;
            for (let j = 0; j < blockLen; j++) {
                const glBlock = gpuShader.glBlocks[j];
                const gpuDescriptorSet = gpuDescriptorSets[glBlock.set];
                const descriptorIndex = gpuDescriptorSet && gpuDescriptorSet.descriptorIndices[glBlock.binding];
                const gpuDescriptor = descriptorIndex >= 0 && gpuDescriptorSet.gpuDescriptors[descriptorIndex];
                if (!gpuDescriptor || !gpuDescriptor.gpuBuffer) {
                    debug_4.error(`Buffer binding '${glBlock.name}' at set ${glBlock.set} binding ${glBlock.binding} is not bounded`);
                    continue;
                }
                const dynamicOffsetIndexSet = dynamicOffsetIndices[glBlock.set];
                const dynamicOffsetIndex = dynamicOffsetIndexSet && dynamicOffsetIndexSet[glBlock.binding];
                let offset = gpuDescriptor.gpuBuffer.glOffset;
                if (dynamicOffsetIndex >= 0) {
                    offset += dynamicOffsets[dynamicOffsetIndex];
                }
                if (cache.glBindUBOs[glBlock.glBinding] !== gpuDescriptor.gpuBuffer.glBuffer
                    || cache.glBindUBOOffsets[glBlock.glBinding] !== offset) {
                    if (offset) {
                        gl.bindBufferRange(gl.UNIFORM_BUFFER, glBlock.glBinding, gpuDescriptor.gpuBuffer.glBuffer, offset, gpuDescriptor.gpuBuffer.size);
                    }
                    else {
                        gl.bindBufferBase(gl.UNIFORM_BUFFER, glBlock.glBinding, gpuDescriptor.gpuBuffer.glBuffer);
                    }
                    cache.glUniformBuffer = cache.glBindUBOs[glBlock.glBinding] = gpuDescriptor.gpuBuffer.glBuffer;
                    cache.glBindUBOOffsets[glBlock.glBinding] = offset;
                }
            }
            const samplerLen = gpuShader.glSamplerTextures.length;
            for (let i = 0; i < samplerLen; i++) {
                const glSampler = gpuShader.glSamplerTextures[i];
                const gpuDescriptorSet = gpuDescriptorSets[glSampler.set];
                let descriptorIndex = gpuDescriptorSet && gpuDescriptorSet.descriptorIndices[glSampler.binding];
                let gpuDescriptor = descriptorIndex >= 0 && gpuDescriptorSet.gpuDescriptors[descriptorIndex];
                for (let l = 0; l < glSampler.units.length; l++) {
                    const texUnit = glSampler.units[l];
                    const glTexUnit = cache.glTexUnits[texUnit];
                    if (!gpuDescriptor || !gpuDescriptor.gpuTexture || !gpuDescriptor.gpuSampler) {
                        debug_4.error(`Sampler binding '${glSampler.name}' at set ${glSampler.set} binding ${glSampler.binding} index ${l} is not bounded`);
                        continue;
                    }
                    if (gpuDescriptor.gpuTexture
                        && gpuDescriptor.gpuTexture.size > 0) {
                        const { gpuTexture } = gpuDescriptor;
                        if (glTexUnit.glTexture !== gpuTexture.glTexture) {
                            if (cache.texUnit !== texUnit) {
                                gl.activeTexture(gl.TEXTURE0 + texUnit);
                                cache.texUnit = texUnit;
                            }
                            if (gpuTexture.glTexture) {
                                gl.bindTexture(gpuTexture.glTarget, gpuTexture.glTexture);
                            }
                            else {
                                gl.bindTexture(gpuTexture.glTarget, device.nullTex2D.gpuTexture.glTexture);
                            }
                            glTexUnit.glTexture = gpuTexture.glTexture;
                        }
                        const { gpuSampler } = gpuDescriptor;
                        if (cache.glSamplerUnits[texUnit] !== gpuSampler.glSampler) {
                            gl.bindSampler(texUnit, gpuSampler.glSampler);
                            cache.glSamplerUnits[texUnit] = gpuSampler.glSampler;
                        }
                    }
                    gpuDescriptor = gpuDescriptorSet.gpuDescriptors[++descriptorIndex];
                }
            }
        } // bind descriptor sets
        // bind vertex/index buffer
        if (gpuInputAssembler && gpuShader
            && (isShaderChanged || gfxStateCache.gpuInputAssembler !== gpuInputAssembler)) {
            gfxStateCache.gpuInputAssembler = gpuInputAssembler;
            if (device.extensions.useVAO) {
                // check vao
                let glVAO = gpuInputAssembler.glVAOs.get(gpuShader.glProgram);
                if (!glVAO) {
                    glVAO = gl.createVertexArray();
                    gpuInputAssembler.glVAOs.set(gpuShader.glProgram, glVAO);
                    gl.bindVertexArray(glVAO);
                    gl.bindBuffer(gl.ARRAY_BUFFER, null);
                    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
                    cache.glArrayBuffer = null;
                    cache.glElementArrayBuffer = null;
                    let glAttrib;
                    for (let j = 0; j < gpuShader.glInputs.length; j++) {
                        const glInput = gpuShader.glInputs[j];
                        glAttrib = null;
                        for (let k = 0; k < gpuInputAssembler.glAttribs.length; k++) {
                            const attrib = gpuInputAssembler.glAttribs[k];
                            if (attrib.name === glInput.name) {
                                glAttrib = attrib;
                                break;
                            }
                        }
                        if (glAttrib) {
                            if (cache.glArrayBuffer !== glAttrib.glBuffer) {
                                gl.bindBuffer(gl.ARRAY_BUFFER, glAttrib.glBuffer);
                                cache.glArrayBuffer = glAttrib.glBuffer;
                            }
                            for (let c = 0; c < glAttrib.componentCount; ++c) {
                                const glLoc = glInput.glLoc + c;
                                const attribOffset = glAttrib.offset + glAttrib.size * c;
                                gl.enableVertexAttribArray(glLoc);
                                cache.glCurrentAttribLocs[glLoc] = true;
                                gl.vertexAttribPointer(glLoc, glAttrib.count, glAttrib.glType, glAttrib.isNormalized, glAttrib.stride, attribOffset);
                                gl.vertexAttribDivisor(glLoc, glAttrib.isInstanced ? 1 : 0);
                            }
                        }
                    }
                    const gpuBuffer = gpuInputAssembler.gpuIndexBuffer;
                    if (gpuBuffer) {
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
                    }
                    gl.bindVertexArray(null);
                    gl.bindBuffer(gl.ARRAY_BUFFER, null);
                    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
                    cache.glArrayBuffer = null;
                    cache.glElementArrayBuffer = null;
                }
                if (cache.glVAO !== glVAO) {
                    gl.bindVertexArray(glVAO);
                    cache.glVAO = glVAO;
                }
            }
            else {
                for (let a = 0; a < device.capabilities.maxVertexAttributes; ++a) {
                    cache.glCurrentAttribLocs[a] = false;
                }
                for (let j = 0; j < gpuShader.glInputs.length; j++) {
                    const glInput = gpuShader.glInputs[j];
                    let glAttrib = null;
                    for (let k = 0; k < gpuInputAssembler.glAttribs.length; k++) {
                        const attrib = gpuInputAssembler.glAttribs[k];
                        if (attrib.name === glInput.name) {
                            glAttrib = attrib;
                            break;
                        }
                    }
                    if (glAttrib) {
                        if (cache.glArrayBuffer !== glAttrib.glBuffer) {
                            gl.bindBuffer(gl.ARRAY_BUFFER, glAttrib.glBuffer);
                            cache.glArrayBuffer = glAttrib.glBuffer;
                        }
                        for (let c = 0; c < glAttrib.componentCount; ++c) {
                            const glLoc = glInput.glLoc + c;
                            const attribOffset = glAttrib.offset + glAttrib.size * c;
                            if (!cache.glEnabledAttribLocs[glLoc] && glLoc >= 0) {
                                gl.enableVertexAttribArray(glLoc);
                                cache.glEnabledAttribLocs[glLoc] = true;
                            }
                            cache.glCurrentAttribLocs[glLoc] = true;
                            gl.vertexAttribPointer(glLoc, glAttrib.count, glAttrib.glType, glAttrib.isNormalized, glAttrib.stride, attribOffset);
                            gl.vertexAttribDivisor(glLoc, glAttrib.isInstanced ? 1 : 0);
                        }
                    }
                } // for
                const gpuBuffer = gpuInputAssembler.gpuIndexBuffer;
                if (gpuBuffer) {
                    if (cache.glElementArrayBuffer !== gpuBuffer.glBuffer) {
                        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer.glBuffer);
                        cache.glElementArrayBuffer = gpuBuffer.glBuffer;
                    }
                }
                for (let a = 0; a < device.capabilities.maxVertexAttributes; ++a) {
                    if (cache.glEnabledAttribLocs[a] !== cache.glCurrentAttribLocs[a]) {
                        gl.disableVertexAttribArray(a);
                        cache.glEnabledAttribLocs[a] = false;
                    }
                }
            }
        } // bind vertex/index buffer
        // update dynamic states
        if (gpuPipelineState && gpuPipelineState.dynamicStates.length) {
            const dsLen = gpuPipelineState.dynamicStates.length;
            for (let k = 0; k < dsLen; k++) {
                const dynamicState = gpuPipelineState.dynamicStates[k];
                switch (dynamicState) {
                    case define_29.DynamicStateFlagBit.LINE_WIDTH: {
                        if (cache.rs.lineWidth !== dynamicStates.lineWidth) {
                            gl.lineWidth(dynamicStates.lineWidth);
                            cache.rs.lineWidth = dynamicStates.lineWidth;
                        }
                        break;
                    }
                    case define_29.DynamicStateFlagBit.DEPTH_BIAS: {
                        if (cache.rs.depthBias !== dynamicStates.depthBiasConstant
                            || cache.rs.depthBiasSlop !== dynamicStates.depthBiasSlope) {
                            gl.polygonOffset(dynamicStates.depthBiasConstant, dynamicStates.depthBiasSlope);
                            cache.rs.depthBias = dynamicStates.depthBiasConstant;
                            cache.rs.depthBiasSlop = dynamicStates.depthBiasSlope;
                        }
                        break;
                    }
                    case define_29.DynamicStateFlagBit.BLEND_CONSTANTS: {
                        const blendConstant = dynamicStates.blendConstant;
                        if ((cache.bs.blendColor.x !== blendConstant.x)
                            || (cache.bs.blendColor.y !== blendConstant.y)
                            || (cache.bs.blendColor.z !== blendConstant.z)
                            || (cache.bs.blendColor.w !== blendConstant.w)) {
                            gl.blendColor(blendConstant.x, blendConstant.y, blendConstant.z, blendConstant.w);
                            cache.bs.blendColor.copy(blendConstant);
                        }
                        break;
                    }
                    case define_29.DynamicStateFlagBit.STENCIL_WRITE_MASK: {
                        const front = dynamicStates.stencilStatesFront;
                        const back = dynamicStates.stencilStatesBack;
                        if (cache.dss.stencilWriteMaskFront !== front.writeMask) {
                            gl.stencilMaskSeparate(gl.FRONT, front.writeMask);
                            cache.dss.stencilWriteMaskFront = front.writeMask;
                        }
                        if (cache.dss.stencilWriteMaskBack !== back.writeMask) {
                            gl.stencilMaskSeparate(gl.BACK, back.writeMask);
                            cache.dss.stencilWriteMaskBack = back.writeMask;
                        }
                        break;
                    }
                    case define_29.DynamicStateFlagBit.STENCIL_COMPARE_MASK: {
                        const front = dynamicStates.stencilStatesFront;
                        const back = dynamicStates.stencilStatesBack;
                        if (cache.dss.stencilRefFront !== front.reference
                            || cache.dss.stencilReadMaskFront !== front.compareMask) {
                            gl.stencilFuncSeparate(gl.FRONT, WebGLCmpFuncs[cache.dss.stencilFuncFront], front.reference, front.compareMask);
                            cache.dss.stencilRefFront = front.reference;
                            cache.dss.stencilReadMaskFront = front.compareMask;
                        }
                        if (cache.dss.stencilRefBack !== back.reference
                            || cache.dss.stencilReadMaskBack !== back.compareMask) {
                            gl.stencilFuncSeparate(gl.BACK, WebGLCmpFuncs[cache.dss.stencilFuncBack], back.reference, back.compareMask);
                            cache.dss.stencilRefBack = back.reference;
                            cache.dss.stencilReadMaskBack = back.compareMask;
                        }
                        break;
                    }
                    default:
                } // switch
            } // for
        } // update dynamic states
    }
    exports_51("WebGL2CmdFuncBindStates", WebGL2CmdFuncBindStates);
    function WebGL2CmdFuncDraw(device, drawInfo) {
        const { gl } = device;
        const { gpuInputAssembler, glPrimitive } = gfxStateCache;
        const md = device.extensions.WEBGL_multi_draw;
        if (gpuInputAssembler) {
            const indexBuffer = gpuInputAssembler.gpuIndexBuffer;
            if (gpuInputAssembler.gpuIndirectBuffer) {
                const { indirects } = gpuInputAssembler.gpuIndirectBuffer;
                if (indirects.drawByIndex) {
                    for (let j = 0; j < indirects.drawCount; j++) {
                        indirects.byteOffsets[j] = indirects.offsets[j] * indexBuffer.stride;
                    }
                    if (md) {
                        if (indirects.instancedDraw) {
                            md.multiDrawElementsInstancedWEBGL(glPrimitive, indirects.counts, 0, gpuInputAssembler.glIndexType, indirects.byteOffsets, 0, indirects.instances, 0, indirects.drawCount);
                        }
                        else {
                            md.multiDrawElementsWEBGL(glPrimitive, indirects.counts, 0, gpuInputAssembler.glIndexType, indirects.byteOffsets, 0, indirects.drawCount);
                        }
                    }
                    else {
                        for (let j = 0; j < indirects.drawCount; j++) {
                            if (indirects.instances[j] > 1) {
                                gl.drawElementsInstanced(glPrimitive, indirects.counts[j], gpuInputAssembler.glIndexType, indirects.byteOffsets[j], indirects.instances[j]);
                            }
                            else {
                                gl.drawElements(glPrimitive, indirects.counts[j], gpuInputAssembler.glIndexType, indirects.byteOffsets[j]);
                            }
                        }
                    }
                }
                else if (md) {
                    if (indirects.instancedDraw) {
                        md.multiDrawArraysInstancedWEBGL(glPrimitive, indirects.offsets, 0, indirects.counts, 0, indirects.instances, 0, indirects.drawCount);
                    }
                    else {
                        md.multiDrawArraysWEBGL(glPrimitive, indirects.offsets, 0, indirects.counts, 0, indirects.drawCount);
                    }
                }
                else {
                    for (let j = 0; j < indirects.drawCount; j++) {
                        if (indirects.instances[j] > 1) {
                            gl.drawArraysInstanced(glPrimitive, indirects.offsets[j], indirects.counts[j], indirects.instances[j]);
                        }
                        else {
                            gl.drawArrays(glPrimitive, indirects.offsets[j], indirects.counts[j]);
                        }
                    }
                }
            }
            else if (drawInfo.instanceCount) {
                if (indexBuffer) {
                    if (drawInfo.indexCount > 0) {
                        const offset = drawInfo.firstIndex * indexBuffer.stride;
                        gl.drawElementsInstanced(glPrimitive, drawInfo.indexCount, gpuInputAssembler.glIndexType, offset, drawInfo.instanceCount);
                    }
                }
                else if (drawInfo.vertexCount > 0) {
                    gl.drawArraysInstanced(glPrimitive, drawInfo.firstVertex, drawInfo.vertexCount, drawInfo.instanceCount);
                }
            }
            else if (indexBuffer) {
                if (drawInfo.indexCount > 0) {
                    const offset = drawInfo.firstIndex * indexBuffer.stride;
                    gl.drawElements(glPrimitive, drawInfo.indexCount, gpuInputAssembler.glIndexType, offset);
                }
            }
            else if (drawInfo.vertexCount > 0) {
                gl.drawArrays(glPrimitive, drawInfo.firstVertex, drawInfo.vertexCount);
            }
        }
    }
    exports_51("WebGL2CmdFuncDraw", WebGL2CmdFuncDraw);
    function WebGL2CmdFuncExecuteCmds(device, cmdPackage) {
        cmdIds.fill(0);
        for (let i = 0; i < cmdPackage.cmds.length; ++i) {
            const cmd = cmdPackage.cmds.array[i];
            const cmdId = cmdIds[cmd]++;
            switch (cmd) {
                case WebGL2Cmd.BEGIN_RENDER_PASS: {
                    const cmd0 = cmdPackage.beginRenderPassCmds.array[cmdId];
                    WebGL2CmdFuncBeginRenderPass(device, cmd0.gpuRenderPass, cmd0.gpuFramebuffer, cmd0.renderArea, cmd0.clearColors, cmd0.clearDepth, cmd0.clearStencil);
                    break;
                }
                /*
                    case WebGL2Cmd.END_RENDER_PASS: {
                        // WebGL 2.0 doesn't support store operation of attachments.
                        // StoreOp.Store is the default GL behavior.
                        break;
                    }
                    */
                case WebGL2Cmd.BIND_STATES: {
                    const cmd2 = cmdPackage.bindStatesCmds.array[cmdId];
                    WebGL2CmdFuncBindStates(device, cmd2.gpuPipelineState, cmd2.gpuInputAssembler, cmd2.gpuDescriptorSets, cmd2.dynamicOffsets, cmd2.dynamicStates);
                    break;
                }
                case WebGL2Cmd.DRAW: {
                    const cmd3 = cmdPackage.drawCmds.array[cmdId];
                    WebGL2CmdFuncDraw(device, cmd3.drawInfo);
                    break;
                }
                case WebGL2Cmd.UPDATE_BUFFER: {
                    const cmd4 = cmdPackage.updateBufferCmds.array[cmdId];
                    WebGL2CmdFuncUpdateBuffer(device, cmd4.gpuBuffer, cmd4.buffer, cmd4.offset, cmd4.size);
                    break;
                }
                case WebGL2Cmd.COPY_BUFFER_TO_TEXTURE: {
                    const cmd5 = cmdPackage.copyBufferToTextureCmds.array[cmdId];
                    WebGL2CmdFuncCopyBuffersToTexture(device, cmd5.buffers, cmd5.gpuTexture, cmd5.regions);
                    break;
                }
                default:
            } // switch
        } // for
    }
    exports_51("WebGL2CmdFuncExecuteCmds", WebGL2CmdFuncExecuteCmds);
    function WebGL2CmdFuncCopyTexImagesToTexture(device, texImages, gpuTexture, regions) {
        const { gl } = device;
        const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
        if (glTexUnit.glTexture !== gpuTexture.glTexture) {
            gl.bindTexture(gpuTexture.glTarget, gpuTexture.glTexture);
            glTexUnit.glTexture = gpuTexture.glTexture;
        }
        let n = 0;
        let f = 0;
        switch (gpuTexture.glTarget) {
            case gl.TEXTURE_2D: {
                for (let k = 0; k < regions.length; k++) {
                    const region = regions[k];
                    gl.texSubImage2D(gl.TEXTURE_2D, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, gpuTexture.glFormat, gpuTexture.glType, texImages[n++]);
                }
                break;
            }
            case gl.TEXTURE_CUBE_MAP: {
                for (let k = 0; k < regions.length; k++) {
                    const region = regions[k];
                    const fcount = region.texSubres.baseArrayLayer + region.texSubres.layerCount;
                    for (f = region.texSubres.baseArrayLayer; f < fcount; ++f) {
                        gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, gpuTexture.glFormat, gpuTexture.glType, texImages[n++]);
                    }
                }
                break;
            }
            default: {
                console.error('Unsupported GL texture type, copy buffer to texture failed.');
            }
        }
        if (gpuTexture.flags & define_29.TextureFlagBit.GEN_MIPMAP) {
            gl.generateMipmap(gpuTexture.glTarget);
        }
    }
    exports_51("WebGL2CmdFuncCopyTexImagesToTexture", WebGL2CmdFuncCopyTexImagesToTexture);
    function WebGL2CmdFuncCopyBuffersToTexture(device, buffers, gpuTexture, regions) {
        const { gl } = device;
        const glTexUnit = device.stateCache.glTexUnits[device.stateCache.texUnit];
        if (glTexUnit.glTexture !== gpuTexture.glTexture) {
            gl.bindTexture(gpuTexture.glTarget, gpuTexture.glTexture);
            glTexUnit.glTexture = gpuTexture.glTexture;
        }
        let n = 0;
        let w = 1;
        let h = 1;
        let f = 0;
        const fmtInfo = define_29.FormatInfos[gpuTexture.format];
        const { isCompressed } = fmtInfo;
        switch (gpuTexture.glTarget) {
            case gl.TEXTURE_2D: {
                for (let k = 0; k < regions.length; k++) {
                    const region = regions[k];
                    w = region.texExtent.width;
                    h = region.texExtent.height;
                    const pixels = buffers[n++];
                    if (!isCompressed) {
                        gl.texSubImage2D(gl.TEXTURE_2D, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, w, h, gpuTexture.glFormat, gpuTexture.glType, pixels);
                    }
                    else if (gpuTexture.glInternalFmt !== webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_ETC1_WEBGL) {
                        gl.compressedTexSubImage2D(gl.TEXTURE_2D, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, w, h, gpuTexture.glFormat, pixels);
                    }
                    else { // WEBGL_compressed_texture_etc1
                        gl.compressedTexImage2D(gl.TEXTURE_2D, region.texSubres.mipLevel, gpuTexture.glInternalFmt, w, h, 0, pixels);
                    }
                }
                break;
            }
            case gl.TEXTURE_CUBE_MAP: {
                for (let k = 0; k < regions.length; k++) {
                    const region = regions[k];
                    const fcount = region.texSubres.baseArrayLayer + region.texSubres.layerCount;
                    for (f = region.texSubres.baseArrayLayer; f < fcount; ++f) {
                        w = region.texExtent.width;
                        h = region.texExtent.height;
                        const pixels = buffers[n++];
                        if (!isCompressed) {
                            gl.texSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, w, h, gpuTexture.glFormat, gpuTexture.glType, pixels);
                        }
                        else if (gpuTexture.glInternalFmt !== webgl2_define_1.WebGL2EXT.COMPRESSED_RGB_ETC1_WEBGL) {
                            gl.compressedTexSubImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, region.texSubres.mipLevel, region.texOffset.x, region.texOffset.y, w, h, gpuTexture.glFormat, pixels);
                        }
                        else { // WEBGL_compressed_texture_etc1
                            gl.compressedTexImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X + f, region.texSubres.mipLevel, gpuTexture.glInternalFmt, w, h, 0, pixels);
                        }
                    }
                }
                break;
            }
            default: {
                console.error('Unsupported GL texture type, copy buffer to texture failed.');
            }
        }
        if (gpuTexture.flags & define_29.TextureFlagBit.GEN_MIPMAP) {
            gl.generateMipmap(gpuTexture.glTarget);
        }
    }
    exports_51("WebGL2CmdFuncCopyBuffersToTexture", WebGL2CmdFuncCopyBuffersToTexture);
    function WebGL2CmdFuncCopyTextureToBuffers(device, gpuTexture, buffers, regions) {
        const { gl } = device;
        const cache = device.stateCache;
        const framebuffer = gl.createFramebuffer();
        gl.bindFramebuffer(gl.FRAMEBUFFER, framebuffer);
        let x = 0;
        let y = 0;
        let w = 1;
        let h = 1;
        switch (gpuTexture.glTarget) {
            case gl.TEXTURE_2D: {
                for (let k = 0; k < regions.length; k++) {
                    const region = regions[k];
                    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gpuTexture.glTarget, gpuTexture.glTexture, region.texSubres.mipLevel);
                    x = region.texOffset.x;
                    y = region.texOffset.y;
                    w = region.texExtent.width;
                    h = region.texExtent.height;
                    gl.readPixels(x, y, w, h, gpuTexture.glFormat, gpuTexture.glType, buffers[k]);
                }
                break;
            }
            default: {
                console.error('Unsupported GL texture type, copy texture to buffers failed.');
            }
        }
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        cache.glFramebuffer = null;
        gl.deleteFramebuffer(framebuffer);
    }
    exports_51("WebGL2CmdFuncCopyTextureToBuffers", WebGL2CmdFuncCopyTextureToBuffers);
    function WebGL2CmdFuncBlitFramebuffer(device, src, dst, srcRect, dstRect, filter) {
        const { gl } = device;
        if (device.stateCache.glReadFramebuffer !== src.glFramebuffer) {
            gl.bindFramebuffer(gl.READ_FRAMEBUFFER, src.glFramebuffer);
            device.stateCache.glReadFramebuffer = src.glFramebuffer;
        }
        const rebindFBO = (dst.glFramebuffer !== device.stateCache.glFramebuffer);
        if (rebindFBO) {
            gl.bindFramebuffer(gl.DRAW_FRAMEBUFFER, dst.glFramebuffer);
        }
        let mask = 0;
        if (src.gpuColorTextures.length > 0) {
            mask |= gl.COLOR_BUFFER_BIT;
        }
        if (src.gpuDepthStencilTexture) {
            mask |= gl.DEPTH_BUFFER_BIT;
            if (define_29.FormatInfos[src.gpuDepthStencilTexture.format].hasStencil) {
                mask |= gl.STENCIL_BUFFER_BIT;
            }
        }
        const glFilter = (filter === define_29.Filter.LINEAR || filter === define_29.Filter.ANISOTROPIC) ? gl.LINEAR : gl.NEAREST;
        gl.blitFramebuffer(srcRect.x, srcRect.y, srcRect.x + srcRect.width, srcRect.y + srcRect.height, dstRect.x, dstRect.y, dstRect.x + dstRect.width, dstRect.y + dstRect.height, mask, glFilter);
        if (rebindFBO) {
            gl.bindFramebuffer(gl.FRAMEBUFFER, device.stateCache.glFramebuffer);
        }
    }
    exports_51("WebGL2CmdFuncBlitFramebuffer", WebGL2CmdFuncBlitFramebuffer);
    return {
        setters: [
            function (cached_array_4_1) {
                cached_array_4 = cached_array_4_1;
            },
            function (debug_4_1) {
                debug_4 = debug_4_1;
            },
            function (define_29_1) {
                define_29 = define_29_1;
            },
            function (webgl2_define_1_1) {
                webgl2_define_1 = webgl2_define_1_1;
            }
        ],
        execute: function () {
            WebGLWraps = [
                0x2901,
                0x8370,
                0x812F,
                0x812F, // WebGLRenderingContext.CLAMP_TO_EDGE
            ];
            _f32v4 = new Float32Array(4);
            WebGLCmpFuncs = [
                0x0200,
                0x0201,
                0x0202,
                0x0203,
                0x0204,
                0x0205,
                0x0206,
                0x0207, // WebGLRenderingContext.ALWAYS,
            ];
            WebGLStencilOps = [
                0x0000,
                0x1E00,
                0x1E01,
                0x1E02,
                0x1E03,
                0x150A,
                0x8507,
                0x8508, // WebGLRenderingContext.DECR_WRAP,
            ];
            WebGLBlendOps = [
                0x8006,
                0x800A,
                0x800B,
                0x8007,
                0x8008, // WebGL2RenderingContext.MAX,
            ];
            WebGLBlendFactors = [
                0x0000,
                0x0001,
                0x0302,
                0x0304,
                0x0303,
                0x0305,
                0x0300,
                0x0306,
                0x0301,
                0x0307,
                0x0308,
                0x8001,
                0x8002,
                0x8003,
                0x8004, // WebGLRenderingContext.ONE_MINUS_CONSTANT_ALPHA,
            ];
            (function (WebGL2Cmd) {
                WebGL2Cmd[WebGL2Cmd["BEGIN_RENDER_PASS"] = 0] = "BEGIN_RENDER_PASS";
                WebGL2Cmd[WebGL2Cmd["END_RENDER_PASS"] = 1] = "END_RENDER_PASS";
                WebGL2Cmd[WebGL2Cmd["BIND_STATES"] = 2] = "BIND_STATES";
                WebGL2Cmd[WebGL2Cmd["DRAW"] = 3] = "DRAW";
                WebGL2Cmd[WebGL2Cmd["UPDATE_BUFFER"] = 4] = "UPDATE_BUFFER";
                WebGL2Cmd[WebGL2Cmd["COPY_BUFFER_TO_TEXTURE"] = 5] = "COPY_BUFFER_TO_TEXTURE";
                WebGL2Cmd[WebGL2Cmd["COUNT"] = 6] = "COUNT";
            })(WebGL2Cmd || (WebGL2Cmd = {}));
            exports_51("WebGL2Cmd", WebGL2Cmd);
            WebGL2CmdObject = class WebGL2CmdObject {
                constructor(type) {
                    this.refCount = 0;
                    this.cmdType = type;
                }
            };
            exports_51("WebGL2CmdObject", WebGL2CmdObject);
            WebGL2CmdBeginRenderPass = class WebGL2CmdBeginRenderPass extends WebGL2CmdObject {
                constructor() {
                    super(WebGL2Cmd.BEGIN_RENDER_PASS);
                    this.gpuRenderPass = null;
                    this.gpuFramebuffer = null;
                    this.renderArea = new define_29.Rect();
                    this.clearColors = [];
                    this.clearDepth = 1.0;
                    this.clearStencil = 0;
                }
                clear() {
                    this.gpuFramebuffer = null;
                    this.clearColors.length = 0;
                }
            };
            exports_51("WebGL2CmdBeginRenderPass", WebGL2CmdBeginRenderPass);
            WebGL2CmdBindStates = class WebGL2CmdBindStates extends WebGL2CmdObject {
                constructor() {
                    super(WebGL2Cmd.BIND_STATES);
                    this.gpuPipelineState = null;
                    this.gpuInputAssembler = null;
                    this.gpuDescriptorSets = [];
                    this.dynamicOffsets = [];
                    this.dynamicStates = new define_29.DynamicStates();
                }
                clear() {
                    this.gpuPipelineState = null;
                    this.gpuInputAssembler = null;
                    this.gpuDescriptorSets.length = 0;
                    this.dynamicOffsets.length = 0;
                }
            };
            exports_51("WebGL2CmdBindStates", WebGL2CmdBindStates);
            WebGL2CmdDraw = class WebGL2CmdDraw extends WebGL2CmdObject {
                constructor() {
                    super(WebGL2Cmd.DRAW);
                    this.drawInfo = new define_29.DrawInfo();
                }
                clear() {
                }
            };
            exports_51("WebGL2CmdDraw", WebGL2CmdDraw);
            WebGL2CmdUpdateBuffer = class WebGL2CmdUpdateBuffer extends WebGL2CmdObject {
                constructor() {
                    super(WebGL2Cmd.UPDATE_BUFFER);
                    this.gpuBuffer = null;
                    this.buffer = null;
                    this.offset = 0;
                    this.size = 0;
                }
                clear() {
                    this.gpuBuffer = null;
                    this.buffer = null;
                }
            };
            exports_51("WebGL2CmdUpdateBuffer", WebGL2CmdUpdateBuffer);
            WebGL2CmdCopyBufferToTexture = class WebGL2CmdCopyBufferToTexture extends WebGL2CmdObject {
                constructor() {
                    super(WebGL2Cmd.COPY_BUFFER_TO_TEXTURE);
                    this.gpuTexture = null;
                    this.buffers = [];
                    this.regions = [];
                }
                clear() {
                    this.gpuTexture = null;
                    this.buffers.length = 0;
                    this.regions.length = 0;
                }
            };
            exports_51("WebGL2CmdCopyBufferToTexture", WebGL2CmdCopyBufferToTexture);
            WebGL2CmdPackage = class WebGL2CmdPackage {
                constructor() {
                    this.cmds = new cached_array_4.CachedArray(1);
                    this.beginRenderPassCmds = new cached_array_4.CachedArray(1);
                    this.bindStatesCmds = new cached_array_4.CachedArray(1);
                    this.drawCmds = new cached_array_4.CachedArray(1);
                    this.updateBufferCmds = new cached_array_4.CachedArray(1);
                    this.copyBufferToTextureCmds = new cached_array_4.CachedArray(1);
                }
                clearCmds(allocator) {
                    if (this.beginRenderPassCmds.length) {
                        allocator.beginRenderPassCmdPool.freeCmds(this.beginRenderPassCmds);
                        this.beginRenderPassCmds.clear();
                    }
                    if (this.bindStatesCmds.length) {
                        allocator.bindStatesCmdPool.freeCmds(this.bindStatesCmds);
                        this.bindStatesCmds.clear();
                    }
                    if (this.drawCmds.length) {
                        allocator.drawCmdPool.freeCmds(this.drawCmds);
                        this.drawCmds.clear();
                    }
                    if (this.updateBufferCmds.length) {
                        allocator.updateBufferCmdPool.freeCmds(this.updateBufferCmds);
                        this.updateBufferCmds.clear();
                    }
                    if (this.copyBufferToTextureCmds.length) {
                        allocator.copyBufferToTextureCmdPool.freeCmds(this.copyBufferToTextureCmds);
                        this.copyBufferToTextureCmds.clear();
                    }
                    this.cmds.clear();
                }
            };
            exports_51("WebGL2CmdPackage", WebGL2CmdPackage);
            gfxStateCache = {
                gpuPipelineState: null,
                gpuInputAssembler: null,
                glPrimitive: 0,
                invalidateAttachments: [],
            };
            cmdIds = new Array(WebGL2Cmd.COUNT);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-buffer", ["engine/cocos/core/gfx/base/buffer", "engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/webgl2/webgl2-commands", "engine/cocos/core/gfx/webgl2/webgl2-define", "engine/cocos/core/gfx/webgl2/webgl2-gpu-objects"], function (exports_52, context_52) {
    "use strict";
    var buffer_2, define_30, webgl2_commands_2, webgl2_define_2, webgl2_gpu_objects_1, WebGL2Buffer;
    var __moduleName = context_52 && context_52.id;
    return {
        setters: [
            function (buffer_2_1) {
                buffer_2 = buffer_2_1;
            },
            function (define_30_1) {
                define_30 = define_30_1;
            },
            function (webgl2_commands_2_1) {
                webgl2_commands_2 = webgl2_commands_2_1;
            },
            function (webgl2_define_2_1) {
                webgl2_define_2 = webgl2_define_2_1;
            },
            function (webgl2_gpu_objects_1_1) {
                webgl2_gpu_objects_1 = webgl2_gpu_objects_1_1;
            }
        ],
        execute: function () {
            WebGL2Buffer = class WebGL2Buffer extends buffer_2.Buffer {
                constructor() {
                    super(...arguments);
                    this._gpuBuffer = null;
                }
                get gpuBuffer() {
                    return this._gpuBuffer;
                }
                initialize(info) {
                    if ('buffer' in info) { // buffer view
                        this._isBufferView = true;
                        const buffer = info.buffer;
                        this._usage = buffer.usage;
                        this._memUsage = buffer.memUsage;
                        this._size = this._stride = info.range;
                        this._count = 1;
                        this._flags = buffer.flags;
                        this._gpuBuffer = {
                            usage: this._usage,
                            memUsage: this._memUsage,
                            size: this._size,
                            stride: this._stride,
                            buffer: null,
                            indirects: buffer.gpuBuffer.indirects,
                            glTarget: buffer.gpuBuffer.glTarget,
                            glBuffer: buffer.gpuBuffer.glBuffer,
                            glOffset: info.offset,
                        };
                    }
                    else { // native buffer
                        this._usage = info.usage;
                        this._memUsage = info.memUsage;
                        this._size = info.size;
                        this._stride = Math.max(info.stride || this._size, 1);
                        this._count = this._size / this._stride;
                        this._flags = info.flags;
                        this._gpuBuffer = {
                            usage: this._usage,
                            memUsage: this._memUsage,
                            size: this._size,
                            stride: this._stride,
                            buffer: null,
                            indirects: new webgl2_gpu_objects_1.WebGL2IndirectDrawInfos(),
                            glTarget: 0,
                            glBuffer: null,
                            glOffset: 0,
                        };
                        webgl2_commands_2.WebGL2CmdFuncCreateBuffer(webgl2_define_2.WebGL2DeviceManager.instance, this._gpuBuffer);
                        webgl2_define_2.WebGL2DeviceManager.instance.memoryStatus.bufferSize += this._size;
                    }
                }
                destroy() {
                    if (this._gpuBuffer) {
                        if (!this._isBufferView) {
                            webgl2_commands_2.WebGL2CmdFuncDestroyBuffer(webgl2_define_2.WebGL2DeviceManager.instance, this._gpuBuffer);
                            webgl2_define_2.WebGL2DeviceManager.instance.memoryStatus.bufferSize -= this._size;
                        }
                        this._gpuBuffer = null;
                    }
                }
                resize(size) {
                    if (this._isBufferView) {
                        console.warn('cannot resize buffer views!');
                        return;
                    }
                    const oldSize = this._size;
                    if (oldSize === size) {
                        return;
                    }
                    this._size = size;
                    this._count = this._size / this._stride;
                    if (this._gpuBuffer) {
                        this._gpuBuffer.size = size;
                        if (size > 0) {
                            webgl2_commands_2.WebGL2CmdFuncResizeBuffer(webgl2_define_2.WebGL2DeviceManager.instance, this._gpuBuffer);
                            webgl2_define_2.WebGL2DeviceManager.instance.memoryStatus.bufferSize -= oldSize;
                            webgl2_define_2.WebGL2DeviceManager.instance.memoryStatus.bufferSize += size;
                        }
                    }
                }
                update(buffer, size) {
                    if (this._isBufferView) {
                        console.warn('cannot update through buffer views!');
                        return;
                    }
                    let buffSize;
                    if (size !== undefined) {
                        buffSize = size;
                    }
                    else if (this._usage & define_30.BufferUsageBit.INDIRECT) {
                        buffSize = 0;
                    }
                    else {
                        buffSize = buffer.byteLength;
                    }
                    webgl2_commands_2.WebGL2CmdFuncUpdateBuffer(webgl2_define_2.WebGL2DeviceManager.instance, this._gpuBuffer, buffer, 0, buffSize);
                }
            };
            exports_52("WebGL2Buffer", WebGL2Buffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/states/webgl2-sampler", ["engine/cocos/core/gfx/base/states/sampler", "engine/cocos/core/gfx/webgl2/webgl2-commands", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_53, context_53) {
    "use strict";
    var sampler_3, webgl2_commands_3, webgl2_define_3, WebGL2Sampler;
    var __moduleName = context_53 && context_53.id;
    return {
        setters: [
            function (sampler_3_1) {
                sampler_3 = sampler_3_1;
            },
            function (webgl2_commands_3_1) {
                webgl2_commands_3 = webgl2_commands_3_1;
            },
            function (webgl2_define_3_1) {
                webgl2_define_3 = webgl2_define_3_1;
            }
        ],
        execute: function () {
            WebGL2Sampler = class WebGL2Sampler extends sampler_3.Sampler {
                constructor(info, hash) {
                    super(info, hash);
                    this._gpuSampler = null;
                    this._gpuSampler = {
                        glSampler: null,
                        minFilter: this._info.minFilter,
                        magFilter: this._info.magFilter,
                        mipFilter: this._info.mipFilter,
                        addressU: this._info.addressU,
                        addressV: this._info.addressV,
                        addressW: this._info.addressW,
                        glMinFilter: 0,
                        glMagFilter: 0,
                        glWrapS: 0,
                        glWrapT: 0,
                        glWrapR: 0,
                    };
                    webgl2_commands_3.WebGL2CmdFuncCreateSampler(webgl2_define_3.WebGL2DeviceManager.instance, this._gpuSampler);
                }
                get gpuSampler() {
                    return this._gpuSampler;
                }
                destroy() {
                    if (this._gpuSampler) {
                        webgl2_commands_3.WebGL2CmdFuncDestroySampler(webgl2_define_3.WebGL2DeviceManager.instance, this._gpuSampler);
                        this._gpuSampler = null;
                    }
                }
            };
            exports_53("WebGL2Sampler", WebGL2Sampler);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-texture", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/base/texture", "engine/cocos/core/gfx/webgl2/webgl2-commands", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_54, context_54) {
    "use strict";
    var define_31, texture_2, webgl2_commands_4, webgl2_define_4, WebGL2Texture;
    var __moduleName = context_54 && context_54.id;
    return {
        setters: [
            function (define_31_1) {
                define_31 = define_31_1;
            },
            function (texture_2_1) {
                texture_2 = texture_2_1;
            },
            function (webgl2_commands_4_1) {
                webgl2_commands_4 = webgl2_commands_4_1;
            },
            function (webgl2_define_4_1) {
                webgl2_define_4 = webgl2_define_4_1;
            }
        ],
        execute: function () {
            WebGL2Texture = class WebGL2Texture extends texture_2.Texture {
                constructor() {
                    super(...arguments);
                    this._gpuTexture = null;
                }
                get gpuTexture() {
                    return this._gpuTexture;
                }
                initialize(info, isSwapchainTexture) {
                    if ('texture' in info) {
                        console.log('WebGL2 does not support texture view.');
                        return;
                    }
                    this._type = info.type;
                    this._usage = info.usage;
                    this._format = info.format;
                    this._width = info.width;
                    this._height = info.height;
                    this._depth = info.depth;
                    this._layerCount = info.layerCount;
                    this._levelCount = info.levelCount;
                    this._samples = info.samples;
                    this._flags = info.flags;
                    this._isPowerOf2 = define_31.IsPowerOf2(this._width) && define_31.IsPowerOf2(this._height);
                    this._size = define_31.FormatSurfaceSize(this._format, this.width, this.height, this.depth, this._levelCount) * this._layerCount;
                    this._gpuTexture = {
                        type: this._type,
                        format: this._format,
                        usage: this._usage,
                        width: this._width,
                        height: this._height,
                        depth: this._depth,
                        size: this._size,
                        arrayLayer: this._layerCount,
                        mipLevel: this._levelCount,
                        samples: this._samples,
                        flags: this._flags,
                        isPowerOf2: this._isPowerOf2,
                        glTarget: 0,
                        glInternalFmt: 0,
                        glFormat: 0,
                        glType: 0,
                        glUsage: 0,
                        glTexture: null,
                        glRenderbuffer: null,
                        glWrapS: 0,
                        glWrapT: 0,
                        glMinFilter: 0,
                        glMagFilter: 0,
                        isSwapchainTexture: isSwapchainTexture || false,
                    };
                    webgl2_commands_4.WebGL2CmdFuncCreateTexture(webgl2_define_4.WebGL2DeviceManager.instance, this._gpuTexture);
                    webgl2_define_4.WebGL2DeviceManager.instance.memoryStatus.textureSize += this._size;
                }
                destroy() {
                    if (this._gpuTexture) {
                        webgl2_commands_4.WebGL2CmdFuncDestroyTexture(webgl2_define_4.WebGL2DeviceManager.instance, this._gpuTexture);
                        webgl2_define_4.WebGL2DeviceManager.instance.memoryStatus.textureSize -= this._size;
                        this._gpuTexture = null;
                    }
                }
                resize(width, height) {
                    const oldSize = this._size;
                    this._width = width;
                    this._height = height;
                    this._size = define_31.FormatSurfaceSize(this._format, this.width, this.height, this.depth, this._levelCount) * this._layerCount;
                    if (this._gpuTexture) {
                        this._gpuTexture.width = width;
                        this._gpuTexture.height = height;
                        this._gpuTexture.size = this._size;
                        webgl2_commands_4.WebGL2CmdFuncResizeTexture(webgl2_define_4.WebGL2DeviceManager.instance, this._gpuTexture);
                        webgl2_define_4.WebGL2DeviceManager.instance.memoryStatus.textureSize -= oldSize;
                        webgl2_define_4.WebGL2DeviceManager.instance.memoryStatus.textureSize += this._size;
                    }
                }
                // ======================= Swapchain Specific ======================= //
                initAsSwapchainTexture(info) {
                    const texInfo = new define_31.TextureInfo();
                    texInfo.format = info.format;
                    texInfo.usage = define_31.FormatInfos[info.format].hasDepth ? define_31.TextureUsageBit.DEPTH_STENCIL_ATTACHMENT : define_31.TextureUsageBit.COLOR_ATTACHMENT;
                    texInfo.width = info.width;
                    texInfo.height = info.height;
                    this.initialize(texInfo, true);
                }
            };
            exports_54("WebGL2Texture", WebGL2Texture);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-descriptor-set-layout", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/base/descriptor-set-layout"], function (exports_55, context_55) {
    "use strict";
    var define_32, descriptor_set_layout_2, WebGL2DescriptorSetLayout;
    var __moduleName = context_55 && context_55.id;
    return {
        setters: [
            function (define_32_1) {
                define_32 = define_32_1;
            },
            function (descriptor_set_layout_2_1) {
                descriptor_set_layout_2 = descriptor_set_layout_2_1;
            }
        ],
        execute: function () {
            WebGL2DescriptorSetLayout = class WebGL2DescriptorSetLayout extends descriptor_set_layout_2.DescriptorSetLayout {
                constructor() {
                    super(...arguments);
                    this._gpuDescriptorSetLayout = null;
                }
                get gpuDescriptorSetLayout() { return this._gpuDescriptorSetLayout; }
                initialize(info) {
                    Array.prototype.push.apply(this._bindings, info.bindings);
                    let descriptorCount = 0;
                    let maxBinding = -1;
                    const flattenedIndices = [];
                    for (let i = 0; i < this._bindings.length; i++) {
                        const binding = this._bindings[i];
                        flattenedIndices.push(descriptorCount);
                        descriptorCount += binding.count;
                        if (binding.binding > maxBinding)
                            maxBinding = binding.binding;
                    }
                    this._bindingIndices = Array(maxBinding + 1).fill(-1);
                    const descriptorIndices = this._descriptorIndices = Array(maxBinding + 1).fill(-1);
                    for (let i = 0; i < this._bindings.length; i++) {
                        const binding = this._bindings[i];
                        this._bindingIndices[binding.binding] = i;
                        descriptorIndices[binding.binding] = flattenedIndices[i];
                    }
                    const dynamicBindings = [];
                    for (let i = 0; i < this._bindings.length; i++) {
                        const binding = this._bindings[i];
                        if (binding.descriptorType & define_32.DESCRIPTOR_DYNAMIC_TYPE) {
                            for (let j = 0; j < binding.count; j++) {
                                dynamicBindings.push(binding.binding);
                            }
                        }
                    }
                    this._gpuDescriptorSetLayout = {
                        bindings: this._bindings,
                        dynamicBindings,
                        descriptorIndices,
                        descriptorCount,
                    };
                }
                destroy() {
                    this._bindings.length = 0;
                }
            };
            exports_55("WebGL2DescriptorSetLayout", WebGL2DescriptorSetLayout);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-descriptor-set", ["engine/cocos/core/gfx/base/descriptor-set", "engine/cocos/core/gfx/base/define"], function (exports_56, context_56) {
    "use strict";
    var descriptor_set_2, define_33, WebGL2DescriptorSet;
    var __moduleName = context_56 && context_56.id;
    return {
        setters: [
            function (descriptor_set_2_1) {
                descriptor_set_2 = descriptor_set_2_1;
            },
            function (define_33_1) {
                define_33 = define_33_1;
            }
        ],
        execute: function () {
            WebGL2DescriptorSet = class WebGL2DescriptorSet extends descriptor_set_2.DescriptorSet {
                constructor() {
                    super(...arguments);
                    this._gpuDescriptorSet = null;
                }
                get gpuDescriptorSet() {
                    return this._gpuDescriptorSet;
                }
                initialize(info) {
                    this._layout = info.layout;
                    const { bindings, descriptorIndices, descriptorCount } = info.layout.gpuDescriptorSetLayout;
                    this._buffers = Array(descriptorCount).fill(null);
                    this._textures = Array(descriptorCount).fill(null);
                    this._samplers = Array(descriptorCount).fill(null);
                    const gpuDescriptors = [];
                    this._gpuDescriptorSet = { gpuDescriptors, descriptorIndices };
                    for (let i = 0; i < bindings.length; ++i) {
                        const binding = bindings[i];
                        for (let j = 0; j < binding.count; j++) {
                            gpuDescriptors.push({
                                type: binding.descriptorType,
                                gpuBuffer: null,
                                gpuTexture: null,
                                gpuSampler: null,
                            });
                        }
                    }
                }
                destroy() {
                    this._layout = null;
                    this._gpuDescriptorSet = null;
                }
                update() {
                    if (this._isDirty && this._gpuDescriptorSet) {
                        const descriptors = this._gpuDescriptorSet.gpuDescriptors;
                        for (let i = 0; i < descriptors.length; ++i) {
                            if (descriptors[i].type & define_33.DESCRIPTOR_BUFFER_TYPE) {
                                if (this._buffers[i]) {
                                    descriptors[i].gpuBuffer = this._buffers[i].gpuBuffer;
                                }
                            }
                            else if (descriptors[i].type & define_33.DESCRIPTOR_SAMPLER_TYPE) {
                                if (this._textures[i]) {
                                    descriptors[i].gpuTexture = this._textures[i].gpuTexture;
                                }
                                if (this._samplers[i]) {
                                    descriptors[i].gpuSampler = this._samplers[i].gpuSampler;
                                }
                            }
                        }
                        this._isDirty = false;
                    }
                }
            };
            exports_56("WebGL2DescriptorSet", WebGL2DescriptorSet);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-render-pass", ["engine/cocos/core/gfx/base/render-pass"], function (exports_57, context_57) {
    "use strict";
    var render_pass_2, WebGL2RenderPass;
    var __moduleName = context_57 && context_57.id;
    return {
        setters: [
            function (render_pass_2_1) {
                render_pass_2 = render_pass_2_1;
            }
        ],
        execute: function () {
            WebGL2RenderPass = class WebGL2RenderPass extends render_pass_2.RenderPass {
                constructor() {
                    super(...arguments);
                    this._gpuRenderPass = null;
                }
                get gpuRenderPass() {
                    return this._gpuRenderPass;
                }
                initialize(info) {
                    this._colorInfos = info.colorAttachments;
                    this._depthStencilInfo = info.depthStencilAttachment;
                    this._subpasses = info.subpasses;
                    this._gpuRenderPass = {
                        colorAttachments: this._colorInfos,
                        depthStencilAttachment: this._depthStencilInfo,
                    };
                    this._hash = this.computeHash();
                }
                destroy() {
                    this._gpuRenderPass = null;
                }
            };
            exports_57("WebGL2RenderPass", WebGL2RenderPass);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-framebuffer", ["engine/cocos/core/gfx/base/framebuffer", "engine/cocos/core/gfx/webgl2/webgl2-commands", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_58, context_58) {
    "use strict";
    var framebuffer_2, webgl2_commands_5, webgl2_define_5, WebGL2Framebuffer;
    var __moduleName = context_58 && context_58.id;
    return {
        setters: [
            function (framebuffer_2_1) {
                framebuffer_2 = framebuffer_2_1;
            },
            function (webgl2_commands_5_1) {
                webgl2_commands_5 = webgl2_commands_5_1;
            },
            function (webgl2_define_5_1) {
                webgl2_define_5 = webgl2_define_5_1;
            }
        ],
        execute: function () {
            WebGL2Framebuffer = class WebGL2Framebuffer extends framebuffer_2.Framebuffer {
                constructor() {
                    super(...arguments);
                    this._gpuFramebuffer = null;
                }
                get gpuFramebuffer() {
                    return this._gpuFramebuffer;
                }
                initialize(info) {
                    this._renderPass = info.renderPass;
                    this._colorTextures = info.colorTextures || [];
                    this._depthStencilTexture = info.depthStencilTexture || null;
                    const gpuColorTextures = [];
                    for (let i = 0; i < info.colorTextures.length; i++) {
                        const colorTexture = info.colorTextures[i];
                        if (colorTexture) {
                            gpuColorTextures.push(colorTexture.gpuTexture);
                        }
                    }
                    let gpuDepthStencilTexture = null;
                    if (info.depthStencilTexture) {
                        gpuDepthStencilTexture = info.depthStencilTexture.gpuTexture;
                    }
                    let width = Number.MAX_SAFE_INTEGER;
                    let height = Number.MAX_SAFE_INTEGER;
                    this._gpuFramebuffer = {
                        gpuRenderPass: info.renderPass.gpuRenderPass,
                        gpuColorTextures,
                        gpuDepthStencilTexture,
                        glFramebuffer: null,
                        isOffscreen: true,
                        get width() {
                            return this.isOffscreen ? width : this.gpuColorTextures[0].width;
                        },
                        set width(val) {
                            width = val;
                        },
                        get height() {
                            return this.isOffscreen ? height : this.gpuColorTextures[0].height;
                        },
                        set height(val) {
                            height = val;
                        },
                    };
                    webgl2_commands_5.WebGL2CmdFuncCreateFramebuffer(webgl2_define_5.WebGL2DeviceManager.instance, this._gpuFramebuffer);
                }
                destroy() {
                    if (this._gpuFramebuffer) {
                        webgl2_commands_5.WebGL2CmdFuncDestroyFramebuffer(webgl2_define_5.WebGL2DeviceManager.instance, this._gpuFramebuffer);
                        this._gpuFramebuffer = null;
                    }
                }
            };
            exports_58("WebGL2Framebuffer", WebGL2Framebuffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-input-assembler", ["engine/cocos/core/gfx/base/input-assembler", "engine/cocos/core/gfx/webgl2/webgl2-commands", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_59, context_59) {
    "use strict";
    var input_assembler_2, webgl2_commands_6, webgl2_define_6, WebGL2InputAssembler;
    var __moduleName = context_59 && context_59.id;
    return {
        setters: [
            function (input_assembler_2_1) {
                input_assembler_2 = input_assembler_2_1;
            },
            function (webgl2_commands_6_1) {
                webgl2_commands_6 = webgl2_commands_6_1;
            },
            function (webgl2_define_6_1) {
                webgl2_define_6 = webgl2_define_6_1;
            }
        ],
        execute: function () {
            WebGL2InputAssembler = class WebGL2InputAssembler extends input_assembler_2.InputAssembler {
                constructor() {
                    super(...arguments);
                    this._gpuInputAssembler = null;
                }
                get gpuInputAssembler() {
                    return this._gpuInputAssembler;
                }
                initialize(info) {
                    if (info.vertexBuffers.length === 0) {
                        console.error('InputAssemblerInfo.vertexBuffers is null.');
                        return;
                    }
                    this._attributes = info.attributes;
                    this._attributesHash = this.computeAttributesHash();
                    this._vertexBuffers = info.vertexBuffers;
                    if (info.indexBuffer) {
                        this._indexBuffer = info.indexBuffer;
                        this._drawInfo.indexCount = this._indexBuffer.size / this._indexBuffer.stride;
                        this._drawInfo.firstIndex = 0;
                    }
                    else {
                        const vertBuff = this._vertexBuffers[0];
                        this._drawInfo.vertexCount = vertBuff.size / vertBuff.stride;
                        this._drawInfo.firstVertex = 0;
                        this._drawInfo.vertexOffset = 0;
                    }
                    this._drawInfo.instanceCount = 0;
                    this._drawInfo.firstInstance = 0;
                    this._indirectBuffer = info.indirectBuffer || null;
                    const gpuVertexBuffers = new Array(info.vertexBuffers.length);
                    for (let i = 0; i < info.vertexBuffers.length; ++i) {
                        const vb = info.vertexBuffers[i];
                        if (vb.gpuBuffer) {
                            gpuVertexBuffers[i] = vb.gpuBuffer;
                        }
                    }
                    let gpuIndexBuffer = null;
                    let glIndexType = 0;
                    if (info.indexBuffer) {
                        gpuIndexBuffer = info.indexBuffer.gpuBuffer;
                        if (gpuIndexBuffer) {
                            switch (gpuIndexBuffer.stride) {
                                case 1:
                                    glIndexType = 0x1401;
                                    break; // WebGLRenderingContext.UNSIGNED_BYTE
                                case 2:
                                    glIndexType = 0x1403;
                                    break; // WebGLRenderingContext.UNSIGNED_SHORT
                                case 4:
                                    glIndexType = 0x1405;
                                    break; // WebGLRenderingContext.UNSIGNED_INT
                                default: {
                                    console.error('Illegal index buffer stride.');
                                }
                            }
                        }
                    }
                    let gpuIndirectBuffer = null;
                    if (info.indirectBuffer) {
                        gpuIndirectBuffer = info.indirectBuffer.gpuBuffer;
                    }
                    this._gpuInputAssembler = {
                        attributes: info.attributes,
                        gpuVertexBuffers,
                        gpuIndexBuffer,
                        gpuIndirectBuffer,
                        glAttribs: [],
                        glIndexType,
                        glVAOs: new Map(),
                    };
                    webgl2_commands_6.WebGL2CmdFuncCreateInputAssember(webgl2_define_6.WebGL2DeviceManager.instance, this._gpuInputAssembler);
                }
                destroy() {
                    const device = webgl2_define_6.WebGL2DeviceManager.instance;
                    if (this._gpuInputAssembler && device.extensions.useVAO) {
                        webgl2_commands_6.WebGL2CmdFuncDestroyInputAssembler(device, this._gpuInputAssembler);
                    }
                    this._gpuInputAssembler = null;
                }
            };
            exports_59("WebGL2InputAssembler", WebGL2InputAssembler);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-shader", ["engine/cocos/core/gfx/base/shader", "engine/cocos/core/gfx/webgl2/webgl2-commands", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_60, context_60) {
    "use strict";
    var shader_2, webgl2_commands_7, webgl2_define_7, WebGL2Shader;
    var __moduleName = context_60 && context_60.id;
    return {
        setters: [
            function (shader_2_1) {
                shader_2 = shader_2_1;
            },
            function (webgl2_commands_7_1) {
                webgl2_commands_7 = webgl2_commands_7_1;
            },
            function (webgl2_define_7_1) {
                webgl2_define_7 = webgl2_define_7_1;
            }
        ],
        execute: function () {
            WebGL2Shader = class WebGL2Shader extends shader_2.Shader {
                constructor() {
                    super(...arguments);
                    this._gpuShader = null;
                }
                get gpuShader() {
                    return this._gpuShader;
                }
                initialize(info) {
                    this._name = info.name;
                    this._stages = info.stages;
                    this._attributes = info.attributes;
                    this._blocks = info.blocks;
                    this._samplers = info.samplers;
                    this._gpuShader = {
                        name: info.name,
                        blocks: info.blocks.slice(),
                        samplerTextures: info.samplerTextures.slice(),
                        subpassInputs: info.subpassInputs.slice(),
                        gpuStages: new Array(info.stages.length),
                        glProgram: null,
                        glInputs: [],
                        glUniforms: [],
                        glBlocks: [],
                        glSamplerTextures: [],
                    };
                    for (let i = 0; i < info.stages.length; ++i) {
                        const stage = info.stages[i];
                        this._gpuShader.gpuStages[i] = {
                            type: stage.stage,
                            source: stage.source,
                            glShader: null,
                        };
                    }
                    webgl2_commands_7.WebGL2CmdFuncCreateShader(webgl2_define_7.WebGL2DeviceManager.instance, this._gpuShader);
                }
                destroy() {
                    if (this._gpuShader) {
                        webgl2_commands_7.WebGL2CmdFuncDestroyShader(webgl2_define_7.WebGL2DeviceManager.instance, this._gpuShader);
                        this._gpuShader = null;
                    }
                }
            };
            exports_60("WebGL2Shader", WebGL2Shader);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-pipeline-layout", ["engine/cocos/core/gfx/base/pipeline-layout"], function (exports_61, context_61) {
    "use strict";
    var pipeline_layout_2, WebGL2PipelineLayout;
    var __moduleName = context_61 && context_61.id;
    return {
        setters: [
            function (pipeline_layout_2_1) {
                pipeline_layout_2 = pipeline_layout_2_1;
            }
        ],
        execute: function () {
            WebGL2PipelineLayout = class WebGL2PipelineLayout extends pipeline_layout_2.PipelineLayout {
                constructor() {
                    super(...arguments);
                    this._gpuPipelineLayout = null;
                }
                get gpuPipelineLayout() { return this._gpuPipelineLayout; }
                initialize(info) {
                    Array.prototype.push.apply(this._setLayouts, info.setLayouts);
                    const dynamicOffsetIndices = [];
                    const gpuSetLayouts = [];
                    let dynamicOffsetCount = 0;
                    const dynamicOffsetOffsets = [];
                    for (let i = 0; i < this._setLayouts.length; i++) {
                        const setLayout = this._setLayouts[i];
                        const dynamicBindings = setLayout.gpuDescriptorSetLayout.dynamicBindings;
                        const indices = Array(setLayout.bindingIndices.length).fill(-1);
                        for (let j = 0; j < dynamicBindings.length; j++) {
                            const binding = dynamicBindings[j];
                            if (indices[binding] < 0)
                                indices[binding] = dynamicOffsetCount + j;
                        }
                        gpuSetLayouts.push(setLayout.gpuDescriptorSetLayout);
                        dynamicOffsetIndices.push(indices);
                        dynamicOffsetOffsets.push(dynamicOffsetCount);
                        dynamicOffsetCount += dynamicBindings.length;
                    }
                    this._gpuPipelineLayout = {
                        gpuSetLayouts,
                        dynamicOffsetIndices,
                        dynamicOffsetCount,
                        dynamicOffsetOffsets,
                    };
                }
                destroy() {
                    this._setLayouts.length = 0;
                }
            };
            exports_61("WebGL2PipelineLayout", WebGL2PipelineLayout);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-pipeline-state", ["engine/cocos/core/gfx/base/pipeline-state"], function (exports_62, context_62) {
    "use strict";
    var pipeline_state_3, WebGLPrimitives, WebGL2PipelineState;
    var __moduleName = context_62 && context_62.id;
    return {
        setters: [
            function (pipeline_state_3_1) {
                pipeline_state_3 = pipeline_state_3_1;
            }
        ],
        execute: function () {
            WebGLPrimitives = [
                0x0000,
                0x0001,
                0x0003,
                0x0002,
                0x0000,
                0x0000,
                0x0000,
                0x0004,
                0x0005,
                0x0006,
                0x0000,
                0x0000,
                0x0000,
                0x0000, // WebGLRenderingContext.NONE,
            ];
            WebGL2PipelineState = class WebGL2PipelineState extends pipeline_state_3.PipelineState {
                constructor() {
                    super(...arguments);
                    this._gpuPipelineState = null;
                }
                get gpuPipelineState() {
                    return this._gpuPipelineState;
                }
                initialize(info) {
                    this._primitive = info.primitive;
                    this._shader = info.shader;
                    this._pipelineLayout = info.pipelineLayout;
                    const bs = this._bs;
                    if (info.blendState) {
                        const bsInfo = info.blendState;
                        const { targets } = bsInfo;
                        if (targets) {
                            targets.forEach((t, i) => {
                                bs.setTarget(i, t);
                            });
                        }
                        if (bsInfo.isA2C !== undefined) {
                            bs.isA2C = bsInfo.isA2C;
                        }
                        if (bsInfo.isIndepend !== undefined) {
                            bs.isIndepend = bsInfo.isIndepend;
                        }
                        if (bsInfo.blendColor !== undefined) {
                            bs.blendColor = bsInfo.blendColor;
                        }
                    }
                    Object.assign(this._rs, info.rasterizerState);
                    Object.assign(this._dss, info.depthStencilState);
                    this._is = info.inputState;
                    this._renderPass = info.renderPass;
                    this._dynamicStates = info.dynamicStates;
                    const dynamicStates = [];
                    for (let i = 0; i < 31; i++) {
                        if (this._dynamicStates & (1 << i)) {
                            dynamicStates.push(1 << i);
                        }
                    }
                    this._gpuPipelineState = {
                        glPrimitive: WebGLPrimitives[info.primitive],
                        gpuShader: info.shader.gpuShader,
                        gpuPipelineLayout: info.pipelineLayout.gpuPipelineLayout,
                        rs: info.rasterizerState,
                        dss: info.depthStencilState,
                        bs: info.blendState,
                        gpuRenderPass: info.renderPass.gpuRenderPass,
                        dynamicStates,
                    };
                }
                destroy() {
                    this._gpuPipelineState = null;
                }
            };
            exports_62("WebGL2PipelineState", WebGL2PipelineState);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-command-buffer", ["engine/cocos/core/gfx/base/command-buffer", "engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/webgl2/webgl2-command-allocator", "engine/cocos/core/gfx/webgl2/webgl2-commands", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_63, context_63) {
    "use strict";
    var command_buffer_2, define_34, webgl2_command_allocator_1, webgl2_commands_8, webgl2_define_8, WebGL2CommandBuffer;
    var __moduleName = context_63 && context_63.id;
    return {
        setters: [
            function (command_buffer_2_1) {
                command_buffer_2 = command_buffer_2_1;
            },
            function (define_34_1) {
                define_34 = define_34_1;
            },
            function (webgl2_command_allocator_1_1) {
                webgl2_command_allocator_1 = webgl2_command_allocator_1_1;
            },
            function (webgl2_commands_8_1) {
                webgl2_commands_8 = webgl2_commands_8_1;
            },
            function (webgl2_define_8_1) {
                webgl2_define_8 = webgl2_define_8_1;
            }
        ],
        execute: function () {
            WebGL2CommandBuffer = class WebGL2CommandBuffer extends command_buffer_2.CommandBuffer {
                constructor() {
                    super(...arguments);
                    this.cmdPackage = new webgl2_commands_8.WebGL2CmdPackage();
                    this._cmdAllocator = new webgl2_command_allocator_1.WebGL2CommandAllocator();
                    this._isInRenderPass = false;
                    this._curGPUPipelineState = null;
                    this._curGPUDescriptorSets = [];
                    this._curGPUInputAssembler = null;
                    this._curDynamicOffsets = Array(8).fill(0);
                    this._curDynamicStates = new define_34.DynamicStates();
                    this._isStateInvalied = false;
                }
                initialize(info) {
                    this._type = info.type;
                    this._queue = info.queue;
                    const setCount = webgl2_define_8.WebGL2DeviceManager.instance.bindingMappingInfo.bufferOffsets.length;
                    for (let i = 0; i < setCount; i++) {
                        this._curGPUDescriptorSets.push(null);
                    }
                }
                destroy() {
                    this._cmdAllocator.clearCmds(this.cmdPackage);
                }
                begin(renderPass, subpass = 0, frameBuffer) {
                    this._cmdAllocator.clearCmds(this.cmdPackage);
                    this._curGPUPipelineState = null;
                    this._curGPUInputAssembler = null;
                    this._curGPUDescriptorSets.length = 0;
                    this._numDrawCalls = 0;
                    this._numInstances = 0;
                    this._numTris = 0;
                }
                end() {
                    if (this._isStateInvalied) {
                        this.bindStates();
                    }
                    this._isInRenderPass = false;
                }
                beginRenderPass(renderPass, framebuffer, renderArea, clearColors, clearDepth, clearStencil) {
                    const cmd = this._cmdAllocator.beginRenderPassCmdPool.alloc(webgl2_commands_8.WebGL2CmdBeginRenderPass);
                    cmd.gpuRenderPass = renderPass.gpuRenderPass;
                    cmd.gpuFramebuffer = framebuffer.gpuFramebuffer;
                    cmd.renderArea = renderArea;
                    for (let i = 0; i < clearColors.length; ++i) {
                        cmd.clearColors[i] = clearColors[i];
                    }
                    cmd.clearDepth = clearDepth;
                    cmd.clearStencil = clearStencil;
                    this.cmdPackage.beginRenderPassCmds.push(cmd);
                    this.cmdPackage.cmds.push(webgl2_commands_8.WebGL2Cmd.BEGIN_RENDER_PASS);
                    this._isInRenderPass = true;
                }
                endRenderPass() {
                    this._isInRenderPass = false;
                }
                bindPipelineState(pipelineState) {
                    const gpuPipelineState = pipelineState.gpuPipelineState;
                    if (gpuPipelineState !== this._curGPUPipelineState) {
                        this._curGPUPipelineState = gpuPipelineState;
                        this._isStateInvalied = true;
                    }
                }
                bindDescriptorSet(set, descriptorSet, dynamicOffsets) {
                    var _a;
                    const gpuDescriptorSets = descriptorSet.gpuDescriptorSet;
                    if (gpuDescriptorSets !== this._curGPUDescriptorSets[set]) {
                        this._curGPUDescriptorSets[set] = gpuDescriptorSets;
                        this._isStateInvalied = true;
                    }
                    if (dynamicOffsets) {
                        const gpuPipelineLayout = (_a = this._curGPUPipelineState) === null || _a === void 0 ? void 0 : _a.gpuPipelineLayout;
                        if (gpuPipelineLayout) {
                            const offsets = this._curDynamicOffsets;
                            const idx = gpuPipelineLayout.dynamicOffsetOffsets[set];
                            for (let i = 0; i < dynamicOffsets.length; i++)
                                offsets[idx + i] = dynamicOffsets[i];
                            this._isStateInvalied = true;
                        }
                    }
                }
                bindInputAssembler(inputAssembler) {
                    const gpuInputAssembler = inputAssembler.gpuInputAssembler;
                    this._curGPUInputAssembler = gpuInputAssembler;
                    this._isStateInvalied = true;
                }
                setViewport(viewport) {
                    const cache = this._curDynamicStates.viewport;
                    if (cache.left !== viewport.left
                        || cache.top !== viewport.top
                        || cache.width !== viewport.width
                        || cache.height !== viewport.height
                        || cache.minDepth !== viewport.minDepth
                        || cache.maxDepth !== viewport.maxDepth) {
                        cache.left = viewport.left;
                        cache.top = viewport.top;
                        cache.width = viewport.width;
                        cache.height = viewport.height;
                        cache.minDepth = viewport.minDepth;
                        cache.maxDepth = viewport.maxDepth;
                        this._isStateInvalied = true;
                    }
                }
                setScissor(scissor) {
                    const cache = this._curDynamicStates.scissor;
                    if (cache.x !== scissor.x
                        || cache.y !== scissor.y
                        || cache.width !== scissor.width
                        || cache.height !== scissor.height) {
                        cache.x = scissor.x;
                        cache.y = scissor.y;
                        cache.width = scissor.width;
                        cache.height = scissor.height;
                        this._isStateInvalied = true;
                    }
                }
                setLineWidth(lineWidth) {
                    if (this._curDynamicStates.lineWidth !== lineWidth) {
                        this._curDynamicStates.lineWidth = lineWidth;
                        this._isStateInvalied = true;
                    }
                }
                setDepthBias(depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor) {
                    const cache = this._curDynamicStates;
                    if (cache.depthBiasConstant !== depthBiasConstantFactor
                        || cache.depthBiasClamp !== depthBiasClamp
                        || cache.depthBiasSlope !== depthBiasSlopeFactor) {
                        cache.depthBiasConstant = depthBiasConstantFactor;
                        cache.depthBiasClamp = depthBiasClamp;
                        cache.depthBiasSlope = depthBiasSlopeFactor;
                        this._isStateInvalied = true;
                    }
                }
                setBlendConstants(blendConstants) {
                    const cache = this._curDynamicStates.blendConstant;
                    if (cache.x !== blendConstants.x
                        || cache.y !== blendConstants.y
                        || cache.z !== blendConstants.z
                        || cache.w !== blendConstants.w) {
                        cache.copy(blendConstants);
                        this._isStateInvalied = true;
                    }
                }
                setDepthBound(minDepthBounds, maxDepthBounds) {
                    const cache = this._curDynamicStates;
                    if (cache.depthMinBounds !== minDepthBounds
                        || cache.depthMaxBounds !== maxDepthBounds) {
                        cache.depthMinBounds = minDepthBounds;
                        cache.depthMaxBounds = maxDepthBounds;
                        this._isStateInvalied = true;
                    }
                }
                setStencilWriteMask(face, writeMask) {
                    const front = this._curDynamicStates.stencilStatesFront;
                    const back = this._curDynamicStates.stencilStatesBack;
                    if (face & define_34.StencilFace.FRONT) {
                        if (front.writeMask !== writeMask) {
                            front.writeMask = writeMask;
                            this._isStateInvalied = true;
                        }
                    }
                    if (face & define_34.StencilFace.BACK) {
                        if (back.writeMask !== writeMask) {
                            back.writeMask = writeMask;
                            this._isStateInvalied = true;
                        }
                    }
                }
                setStencilCompareMask(face, reference, compareMask) {
                    const front = this._curDynamicStates.stencilStatesFront;
                    const back = this._curDynamicStates.stencilStatesBack;
                    if (face & define_34.StencilFace.FRONT) {
                        if (front.compareMask !== compareMask
                            || front.reference !== reference) {
                            front.reference = reference;
                            front.compareMask = compareMask;
                            this._isStateInvalied = true;
                        }
                    }
                    if (face & define_34.StencilFace.BACK) {
                        if (back.compareMask !== compareMask
                            || back.reference !== reference) {
                            back.reference = reference;
                            back.compareMask = compareMask;
                            this._isStateInvalied = true;
                        }
                    }
                }
                draw(infoOrAssembler) {
                    if (this._type === define_34.CommandBufferType.PRIMARY && this._isInRenderPass
                        || this._type === define_34.CommandBufferType.SECONDARY) {
                        if (this._isStateInvalied) {
                            this.bindStates();
                        }
                        const info = 'drawInfo' in infoOrAssembler ? infoOrAssembler.drawInfo : infoOrAssembler;
                        const cmd = this._cmdAllocator.drawCmdPool.alloc(webgl2_commands_8.WebGL2CmdDraw);
                        cmd.drawInfo.copy(info);
                        this.cmdPackage.drawCmds.push(cmd);
                        this.cmdPackage.cmds.push(webgl2_commands_8.WebGL2Cmd.DRAW);
                        ++this._numDrawCalls;
                        this._numInstances += info.instanceCount;
                        const indexCount = info.indexCount || info.vertexCount;
                        if (this._curGPUPipelineState) {
                            const glPrimitive = this._curGPUPipelineState.glPrimitive;
                            switch (glPrimitive) {
                                case 0x0004: { // WebGLRenderingContext.TRIANGLES
                                    this._numTris += indexCount / 3 * Math.max(info.instanceCount, 1);
                                    break;
                                }
                                case 0x0005: // WebGLRenderingContext.TRIANGLE_STRIP
                                case 0x0006: { // WebGLRenderingContext.TRIANGLE_FAN
                                    this._numTris += (indexCount - 2) * Math.max(info.instanceCount, 1);
                                    break;
                                }
                                default:
                            }
                        }
                    }
                    else {
                        console.error('Command \'draw\' must be recorded inside a render pass.');
                    }
                }
                updateBuffer(buffer, data, size) {
                    if (this._type === define_34.CommandBufferType.PRIMARY && !this._isInRenderPass
                        || this._type === define_34.CommandBufferType.SECONDARY) {
                        const gpuBuffer = buffer.gpuBuffer;
                        if (gpuBuffer) {
                            const cmd = this._cmdAllocator.updateBufferCmdPool.alloc(webgl2_commands_8.WebGL2CmdUpdateBuffer);
                            let buffSize = 0;
                            let buff = null;
                            // TODO: Have to copy to staging buffer first to make this work for the execution is deferred.
                            // But since we are using specialized primary command buffers in WebGL backends, we leave it as is for now
                            if (buffer.usage & define_34.BufferUsageBit.INDIRECT) {
                                buff = data;
                            }
                            else {
                                if (size !== undefined) {
                                    buffSize = size;
                                }
                                else {
                                    buffSize = data.byteLength;
                                }
                                buff = data;
                            }
                            cmd.gpuBuffer = gpuBuffer;
                            cmd.buffer = buff;
                            cmd.offset = 0;
                            cmd.size = buffSize;
                            this.cmdPackage.updateBufferCmds.push(cmd);
                            this.cmdPackage.cmds.push(webgl2_commands_8.WebGL2Cmd.UPDATE_BUFFER);
                        }
                    }
                    else {
                        console.error('Command \'updateBuffer\' must be recorded outside a render pass.');
                    }
                }
                copyBuffersToTexture(buffers, texture, regions) {
                    if (this._type === define_34.CommandBufferType.PRIMARY && !this._isInRenderPass
                        || this._type === define_34.CommandBufferType.SECONDARY) {
                        const gpuTexture = texture.gpuTexture;
                        if (gpuTexture) {
                            const cmd = this._cmdAllocator.copyBufferToTextureCmdPool.alloc(webgl2_commands_8.WebGL2CmdCopyBufferToTexture);
                            cmd.gpuTexture = gpuTexture;
                            cmd.regions = regions;
                            // TODO: Have to copy to staging buffer first to make this work for the execution is deferred.
                            // But since we are using specialized primary command buffers in WebGL backends, we leave it as is for now
                            cmd.buffers = buffers;
                            this.cmdPackage.copyBufferToTextureCmds.push(cmd);
                            this.cmdPackage.cmds.push(webgl2_commands_8.WebGL2Cmd.COPY_BUFFER_TO_TEXTURE);
                        }
                    }
                    else {
                        console.error('Command \'copyBufferToTexture\' must be recorded outside a render pass.');
                    }
                }
                execute(cmdBuffs, count) {
                    for (let i = 0; i < count; ++i) {
                        const webGL2CmdBuff = cmdBuffs[i];
                        for (let c = 0; c < webGL2CmdBuff.cmdPackage.beginRenderPassCmds.length; ++c) {
                            const cmd = webGL2CmdBuff.cmdPackage.beginRenderPassCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.beginRenderPassCmds.push(cmd);
                        }
                        for (let c = 0; c < webGL2CmdBuff.cmdPackage.bindStatesCmds.length; ++c) {
                            const cmd = webGL2CmdBuff.cmdPackage.bindStatesCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.bindStatesCmds.push(cmd);
                        }
                        for (let c = 0; c < webGL2CmdBuff.cmdPackage.drawCmds.length; ++c) {
                            const cmd = webGL2CmdBuff.cmdPackage.drawCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.drawCmds.push(cmd);
                        }
                        for (let c = 0; c < webGL2CmdBuff.cmdPackage.updateBufferCmds.length; ++c) {
                            const cmd = webGL2CmdBuff.cmdPackage.updateBufferCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.updateBufferCmds.push(cmd);
                        }
                        for (let c = 0; c < webGL2CmdBuff.cmdPackage.copyBufferToTextureCmds.length; ++c) {
                            const cmd = webGL2CmdBuff.cmdPackage.copyBufferToTextureCmds.array[c];
                            ++cmd.refCount;
                            this.cmdPackage.copyBufferToTextureCmds.push(cmd);
                        }
                        this.cmdPackage.cmds.concat(webGL2CmdBuff.cmdPackage.cmds.array);
                        this._numDrawCalls += webGL2CmdBuff._numDrawCalls;
                        this._numInstances += webGL2CmdBuff._numInstances;
                        this._numTris += webGL2CmdBuff._numTris;
                    }
                }
                pipelineBarrier(globalBarrier, textureBarriers, textures) { }
                bindStates() {
                    const bindStatesCmd = this._cmdAllocator.bindStatesCmdPool.alloc(webgl2_commands_8.WebGL2CmdBindStates);
                    bindStatesCmd.gpuPipelineState = this._curGPUPipelineState;
                    Array.prototype.push.apply(bindStatesCmd.gpuDescriptorSets, this._curGPUDescriptorSets);
                    Array.prototype.push.apply(bindStatesCmd.dynamicOffsets, this._curDynamicOffsets);
                    bindStatesCmd.gpuInputAssembler = this._curGPUInputAssembler;
                    bindStatesCmd.dynamicStates = this._curDynamicStates;
                    this.cmdPackage.bindStatesCmds.push(bindStatesCmd);
                    this.cmdPackage.cmds.push(webgl2_commands_8.WebGL2Cmd.BIND_STATES);
                    this._isStateInvalied = false;
                }
            };
            exports_63("WebGL2CommandBuffer", WebGL2CommandBuffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-primary-command-buffer", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/webgl2/webgl2-command-buffer", "engine/cocos/core/gfx/webgl2/webgl2-commands", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_64, context_64) {
    "use strict";
    var define_35, webgl2_command_buffer_1, webgl2_commands_9, webgl2_define_9, WebGL2PrimaryCommandBuffer;
    var __moduleName = context_64 && context_64.id;
    return {
        setters: [
            function (define_35_1) {
                define_35 = define_35_1;
            },
            function (webgl2_command_buffer_1_1) {
                webgl2_command_buffer_1 = webgl2_command_buffer_1_1;
            },
            function (webgl2_commands_9_1) {
                webgl2_commands_9 = webgl2_commands_9_1;
            },
            function (webgl2_define_9_1) {
                webgl2_define_9 = webgl2_define_9_1;
            }
        ],
        execute: function () {
            WebGL2PrimaryCommandBuffer = class WebGL2PrimaryCommandBuffer extends webgl2_command_buffer_1.WebGL2CommandBuffer {
                beginRenderPass(renderPass, framebuffer, renderArea, clearColors, clearDepth, clearStencil) {
                    webgl2_commands_9.WebGL2CmdFuncBeginRenderPass(webgl2_define_9.WebGL2DeviceManager.instance, renderPass.gpuRenderPass, framebuffer.gpuFramebuffer, renderArea, clearColors, clearDepth, clearStencil);
                    this._isInRenderPass = true;
                }
                draw(infoOrAssembler) {
                    if (this._isInRenderPass) {
                        if (this._isStateInvalied) {
                            this.bindStates();
                        }
                        const info = 'drawInfo' in infoOrAssembler ? infoOrAssembler.drawInfo : infoOrAssembler;
                        webgl2_commands_9.WebGL2CmdFuncDraw(webgl2_define_9.WebGL2DeviceManager.instance, info);
                        ++this._numDrawCalls;
                        this._numInstances += info.instanceCount;
                        const indexCount = info.indexCount || info.vertexCount;
                        if (this._curGPUPipelineState) {
                            const glPrimitive = this._curGPUPipelineState.glPrimitive;
                            switch (glPrimitive) {
                                case 0x0004: { // WebGLRenderingContext.TRIANGLES
                                    this._numTris += indexCount / 3 * Math.max(info.instanceCount, 1);
                                    break;
                                }
                                case 0x0005: // WebGLRenderingContext.TRIANGLE_STRIP
                                case 0x0006: { // WebGLRenderingContext.TRIANGLE_FAN
                                    this._numTris += (indexCount - 2) * Math.max(info.instanceCount, 1);
                                    break;
                                }
                                default:
                            }
                        }
                    }
                    else {
                        console.error('Command \'draw\' must be recorded inside a render pass.');
                    }
                }
                setViewport(viewport) {
                    const { stateCache: cache, gl } = webgl2_define_9.WebGL2DeviceManager.instance;
                    if (cache.viewport.left !== viewport.left
                        || cache.viewport.top !== viewport.top
                        || cache.viewport.width !== viewport.width
                        || cache.viewport.height !== viewport.height) {
                        gl.viewport(viewport.left, viewport.top, viewport.width, viewport.height);
                        cache.viewport.left = viewport.left;
                        cache.viewport.top = viewport.top;
                        cache.viewport.width = viewport.width;
                        cache.viewport.height = viewport.height;
                    }
                }
                setScissor(scissor) {
                    const { stateCache: cache, gl } = webgl2_define_9.WebGL2DeviceManager.instance;
                    if (cache.scissorRect.x !== scissor.x
                        || cache.scissorRect.y !== scissor.y
                        || cache.scissorRect.width !== scissor.width
                        || cache.scissorRect.height !== scissor.height) {
                        gl.scissor(scissor.x, scissor.y, scissor.width, scissor.height);
                        cache.scissorRect.x = scissor.x;
                        cache.scissorRect.y = scissor.y;
                        cache.scissorRect.width = scissor.width;
                        cache.scissorRect.height = scissor.height;
                    }
                }
                updateBuffer(buffer, data, size) {
                    if (!this._isInRenderPass) {
                        const gpuBuffer = buffer.gpuBuffer;
                        if (gpuBuffer) {
                            let buffSize;
                            if (size !== undefined) {
                                buffSize = size;
                            }
                            else if (buffer.usage & define_35.BufferUsageBit.INDIRECT) {
                                buffSize = 0;
                            }
                            else {
                                buffSize = data.byteLength;
                            }
                            webgl2_commands_9.WebGL2CmdFuncUpdateBuffer(webgl2_define_9.WebGL2DeviceManager.instance, gpuBuffer, data, 0, buffSize);
                        }
                    }
                    else {
                        console.error('Command \'updateBuffer\' must be recorded outside a render pass.');
                    }
                }
                copyBuffersToTexture(buffers, texture, regions) {
                    if (!this._isInRenderPass) {
                        const gpuTexture = texture.gpuTexture;
                        if (gpuTexture) {
                            webgl2_commands_9.WebGL2CmdFuncCopyBuffersToTexture(webgl2_define_9.WebGL2DeviceManager.instance, buffers, gpuTexture, regions);
                        }
                    }
                    else {
                        console.error('Command \'copyBufferToTexture\' must be recorded outside a render pass.');
                    }
                }
                execute(cmdBuffs, count) {
                    for (let i = 0; i < count; ++i) {
                        // actually they are secondary buffers, the cast here is only for type checking
                        const webGL2CmdBuff = cmdBuffs[i];
                        webgl2_commands_9.WebGL2CmdFuncExecuteCmds(webgl2_define_9.WebGL2DeviceManager.instance, webGL2CmdBuff.cmdPackage);
                        this._numDrawCalls += webGL2CmdBuff._numDrawCalls;
                        this._numInstances += webGL2CmdBuff._numInstances;
                        this._numTris += webGL2CmdBuff._numTris;
                    }
                }
                bindStates() {
                    webgl2_commands_9.WebGL2CmdFuncBindStates(webgl2_define_9.WebGL2DeviceManager.instance, this._curGPUPipelineState, this._curGPUInputAssembler, this._curGPUDescriptorSets, this._curDynamicOffsets, this._curDynamicStates);
                    this._isStateInvalied = false;
                }
            };
            exports_64("WebGL2PrimaryCommandBuffer", WebGL2PrimaryCommandBuffer);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-queue", ["engine/cocos/core/gfx/base/queue"], function (exports_65, context_65) {
    "use strict";
    var queue_2, WebGL2Queue;
    var __moduleName = context_65 && context_65.id;
    return {
        setters: [
            function (queue_2_1) {
                queue_2 = queue_2_1;
            }
        ],
        execute: function () {
            WebGL2Queue = class WebGL2Queue extends queue_2.Queue {
                constructor() {
                    super(...arguments);
                    this.numDrawCalls = 0;
                    this.numInstances = 0;
                    this.numTris = 0;
                }
                initialize(info) {
                    this._type = info.type;
                }
                destroy() {
                }
                submit(cmdBuffs) {
                    for (let i = 0; i < cmdBuffs.length; i++) {
                        const cmdBuff = cmdBuffs[i];
                        // WebGL2CmdFuncExecuteCmds(this._device as WebGL2Device, cmdBuff.cmdPackage); // opted out
                        this.numDrawCalls += cmdBuff.numDrawCalls;
                        this.numInstances += cmdBuff.numInstances;
                        this.numTris += cmdBuff.numTris;
                    }
                }
                clear() {
                    this.numDrawCalls = 0;
                    this.numInstances = 0;
                    this.numTris = 0;
                }
            };
            exports_65("WebGL2Queue", WebGL2Queue);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-state-cache", ["engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/base/pipeline-state"], function (exports_66, context_66) {
    "use strict";
    var define_36, pipeline_state_4, WebGL2StateCache;
    var __moduleName = context_66 && context_66.id;
    return {
        setters: [
            function (define_36_1) {
                define_36 = define_36_1;
            },
            function (pipeline_state_4_1) {
                pipeline_state_4 = pipeline_state_4_1;
            }
        ],
        execute: function () {
            WebGL2StateCache = class WebGL2StateCache {
                constructor() {
                    this.glArrayBuffer = null;
                    this.glElementArrayBuffer = null;
                    this.glUniformBuffer = null;
                    this.glBindUBOs = [];
                    this.glBindUBOOffsets = [];
                    this.glVAO = null;
                    this.texUnit = 0;
                    this.glTexUnits = [];
                    this.glSamplerUnits = [];
                    this.glRenderbuffer = null;
                    this.glFramebuffer = null;
                    this.glReadFramebuffer = null;
                    this.viewport = new define_36.Viewport();
                    this.scissorRect = new define_36.Rect(0, 0, 0, 0);
                    this.rs = new pipeline_state_4.RasterizerState();
                    this.dss = new pipeline_state_4.DepthStencilState();
                    this.bs = new pipeline_state_4.BlendState();
                    this.glProgram = null;
                    this.glEnabledAttribLocs = [];
                    this.glCurrentAttribLocs = [];
                    this.texUnitCacheMap = {};
                }
                initialize(texUnit, bufferBindings, vertexAttributes) {
                    for (let i = 0; i < texUnit; ++i)
                        this.glTexUnits.push({ glTexture: null });
                    this.glSamplerUnits.length = texUnit;
                    this.glSamplerUnits.fill(null);
                    this.glBindUBOs.length = bufferBindings;
                    this.glBindUBOs.fill(null);
                    this.glBindUBOOffsets.length = bufferBindings;
                    this.glBindUBOOffsets.fill(0);
                    this.glEnabledAttribLocs.length = vertexAttributes;
                    this.glEnabledAttribLocs.fill(false);
                    this.glCurrentAttribLocs.length = vertexAttributes;
                    this.glCurrentAttribLocs.fill(false);
                }
            };
            exports_66("WebGL2StateCache", WebGL2StateCache);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-swapchain", ["internal:constants", "engine/cocos/core/platform/debug", "engine/cocos/core/platform/macro", "engine/cocos/core/gfx/webgl2/webgl2-state-cache", "engine/cocos/core/gfx/webgl2/webgl2-texture", "engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/base/swapchain", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_67, context_67) {
    "use strict";
    var internal_constants_4, debug_5, macro_2, webgl2_state_cache_1, webgl2_texture_1, define_37, swapchain_2, webgl2_define_10, eventWebGLContextLost, WebGL2Swapchain;
    var __moduleName = context_67 && context_67.id;
    function initStates(gl) {
        gl.activeTexture(gl.TEXTURE0);
        gl.pixelStorei(gl.PACK_ALIGNMENT, 1);
        gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, false);
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
        // rasterizer state
        gl.enable(gl.SCISSOR_TEST);
        gl.enable(gl.CULL_FACE);
        gl.cullFace(gl.BACK);
        gl.frontFace(gl.CCW);
        gl.polygonOffset(0.0, 0.0);
        // depth stencil state
        gl.enable(gl.DEPTH_TEST);
        gl.depthMask(true);
        gl.depthFunc(gl.LESS);
        gl.stencilFuncSeparate(gl.FRONT, gl.ALWAYS, 1, 0xffff);
        gl.stencilOpSeparate(gl.FRONT, gl.KEEP, gl.KEEP, gl.KEEP);
        gl.stencilMaskSeparate(gl.FRONT, 0xffff);
        gl.stencilFuncSeparate(gl.BACK, gl.ALWAYS, 1, 0xffff);
        gl.stencilOpSeparate(gl.BACK, gl.KEEP, gl.KEEP, gl.KEEP);
        gl.stencilMaskSeparate(gl.BACK, 0xffff);
        gl.disable(gl.STENCIL_TEST);
        // blend state
        gl.disable(gl.SAMPLE_ALPHA_TO_COVERAGE);
        gl.disable(gl.BLEND);
        gl.blendEquationSeparate(gl.FUNC_ADD, gl.FUNC_ADD);
        gl.blendFuncSeparate(gl.ONE, gl.ZERO, gl.ONE, gl.ZERO);
        gl.colorMask(true, true, true, true);
        gl.blendColor(0.0, 0.0, 0.0, 0.0);
    }
    function getExtension(gl, ext) {
        const prefixes = ['', 'WEBKIT_', 'MOZ_'];
        for (let i = 0; i < prefixes.length; ++i) {
            const _ext = gl.getExtension(prefixes[i] + ext);
            if (_ext) {
                // eslint-disable-next-line @typescript-eslint/no-unsafe-return
                return _ext;
            }
        }
        return null;
    }
    function getExtensions(gl) {
        const res = {
            EXT_texture_filter_anisotropic: getExtension(gl, 'EXT_texture_filter_anisotropic'),
            EXT_color_buffer_half_float: getExtension(gl, 'EXT_color_buffer_half_float'),
            EXT_color_buffer_float: getExtension(gl, 'EXT_color_buffer_float'),
            WEBGL_multi_draw: getExtension(gl, 'WEBGL_multi_draw'),
            WEBGL_compressed_texture_etc1: getExtension(gl, 'WEBGL_compressed_texture_etc1'),
            WEBGL_compressed_texture_etc: getExtension(gl, 'WEBGL_compressed_texture_etc'),
            WEBGL_compressed_texture_pvrtc: getExtension(gl, 'WEBGL_compressed_texture_pvrtc'),
            WEBGL_compressed_texture_astc: getExtension(gl, 'WEBGL_compressed_texture_astc'),
            WEBGL_compressed_texture_s3tc: getExtension(gl, 'WEBGL_compressed_texture_s3tc'),
            WEBGL_compressed_texture_s3tc_srgb: getExtension(gl, 'WEBGL_compressed_texture_s3tc_srgb'),
            WEBGL_debug_shaders: getExtension(gl, 'WEBGL_debug_shaders'),
            WEBGL_lose_context: getExtension(gl, 'WEBGL_lose_context'),
            WEBGL_debug_renderer_info: getExtension(gl, 'WEBGL_debug_renderer_info'),
            OES_texture_half_float_linear: getExtension(gl, 'OES_texture_half_float_linear'),
            OES_texture_float_linear: getExtension(gl, 'OES_texture_float_linear'),
            useVAO: true,
        };
        return res;
    }
    exports_67("getExtensions", getExtensions);
    return {
        setters: [
            function (internal_constants_4_1) {
                internal_constants_4 = internal_constants_4_1;
            },
            function (debug_5_1) {
                debug_5 = debug_5_1;
            },
            function (macro_2_1) {
                macro_2 = macro_2_1;
            },
            function (webgl2_state_cache_1_1) {
                webgl2_state_cache_1 = webgl2_state_cache_1_1;
            },
            function (webgl2_texture_1_1) {
                webgl2_texture_1 = webgl2_texture_1_1;
            },
            function (define_37_1) {
                define_37 = define_37_1;
            },
            function (swapchain_2_1) {
                swapchain_2 = swapchain_2_1;
            },
            function (webgl2_define_10_1) {
                webgl2_define_10 = webgl2_define_10_1;
            }
        ],
        execute: function () {
            eventWebGLContextLost = 'webglcontextlost';
            WebGL2Swapchain = class WebGL2Swapchain extends swapchain_2.Swapchain {
                constructor() {
                    super(...arguments);
                    this.stateCache = new webgl2_state_cache_1.WebGL2StateCache();
                    this.nullTex2D = null;
                    this.nullTexCube = null;
                    this._canvas = null;
                    this._webGL2RC = null;
                    this._webGL2ContextLostHandler = null;
                    this._extensions = null;
                }
                get gl() {
                    return this._webGL2RC;
                }
                get extensions() {
                    return this._extensions;
                }
                initialize(info) {
                    this._canvas = info.windowHandle;
                    try {
                        const webGLCtxAttribs = {
                            alpha: macro_2.macro.ENABLE_TRANSPARENT_CANVAS,
                            antialias: internal_constants_4.EDITOR || macro_2.macro.ENABLE_WEBGL_ANTIALIAS,
                            depth: true,
                            stencil: true,
                            premultipliedAlpha: false,
                            preserveDrawingBuffer: false,
                            powerPreference: 'default',
                            failIfMajorPerformanceCaveat: false,
                        };
                        this._webGL2RC = this._canvas.getContext('webgl2', webGLCtxAttribs);
                    }
                    catch (err) {
                        return;
                    }
                    if (!this._webGL2RC) {
                        return;
                    }
                    this._webGL2ContextLostHandler = this._onWebGLContextLost.bind(this);
                    this._canvas.addEventListener(eventWebGLContextLost, this._onWebGLContextLost);
                    const gl = this.gl;
                    this.stateCache.initialize(webgl2_define_10.WebGL2DeviceManager.instance.capabilities.maxTextureUnits, webgl2_define_10.WebGL2DeviceManager.instance.capabilities.maxUniformBufferBindings, webgl2_define_10.WebGL2DeviceManager.instance.capabilities.maxVertexAttributes);
                    this._extensions = getExtensions(gl);
                    // init states
                    initStates(gl);
                    const colorFmt = define_37.Format.RGBA8;
                    let depthStencilFmt = define_37.Format.DEPTH_STENCIL;
                    const depthBits = gl.getParameter(gl.DEPTH_BITS);
                    const stencilBits = gl.getParameter(gl.STENCIL_BITS);
                    if (depthBits && stencilBits)
                        depthStencilFmt = define_37.Format.DEPTH_STENCIL;
                    else if (depthBits)
                        depthStencilFmt = define_37.Format.DEPTH;
                    this._colorTexture = new webgl2_texture_1.WebGL2Texture();
                    // @ts-expect-error(2445) private initializer
                    this._colorTexture.initAsSwapchainTexture({
                        swapchain: this,
                        format: colorFmt,
                        width: info.width,
                        height: info.height,
                    });
                    this._depthStencilTexture = new webgl2_texture_1.WebGL2Texture();
                    // @ts-expect-error(2445) private initializer
                    this._depthStencilTexture.initAsSwapchainTexture({
                        swapchain: this,
                        format: depthStencilFmt,
                        width: info.width,
                        height: info.height,
                    });
                    // create default null texture
                    this.nullTex2D = webgl2_define_10.WebGL2DeviceManager.instance.createTexture(new define_37.TextureInfo(define_37.TextureType.TEX2D, define_37.TextureUsageBit.SAMPLED, define_37.Format.RGBA8, 2, 2, define_37.TextureFlagBit.GEN_MIPMAP));
                    this.nullTexCube = webgl2_define_10.WebGL2DeviceManager.instance.createTexture(new define_37.TextureInfo(define_37.TextureType.CUBE, define_37.TextureUsageBit.SAMPLED, define_37.Format.RGBA8, 2, 2, define_37.TextureFlagBit.GEN_MIPMAP, 6));
                    const nullTexRegion = new define_37.BufferTextureCopy();
                    nullTexRegion.texExtent.width = 2;
                    nullTexRegion.texExtent.height = 2;
                    const nullTexBuff = new Uint8Array(this.nullTex2D.size);
                    nullTexBuff.fill(0);
                    webgl2_define_10.WebGL2DeviceManager.instance.copyBuffersToTexture([nullTexBuff], this.nullTex2D, [nullTexRegion]);
                    nullTexRegion.texSubres.layerCount = 6;
                    webgl2_define_10.WebGL2DeviceManager.instance.copyBuffersToTexture([nullTexBuff, nullTexBuff, nullTexBuff, nullTexBuff, nullTexBuff, nullTexBuff], this.nullTexCube, [nullTexRegion]);
                }
                destroy() {
                    if (this._canvas && this._webGL2ContextLostHandler) {
                        this._canvas.removeEventListener(eventWebGLContextLost, this._webGL2ContextLostHandler);
                        this._webGL2ContextLostHandler = null;
                    }
                    if (this.nullTex2D) {
                        this.nullTex2D.destroy();
                        this.nullTex2D = null;
                    }
                    if (this.nullTexCube) {
                        this.nullTexCube.destroy();
                        this.nullTexCube = null;
                    }
                    this._extensions = null;
                    this._webGL2RC = null;
                    this._canvas = null;
                }
                resize(width, height, surfaceTransform) {
                    if (this._colorTexture.width !== width || this._colorTexture.height !== height) {
                        debug_5.debug(`Resizing swapchain: ${width}x${height}`);
                        this._canvas.width = width;
                        this._canvas.height = height;
                        this._colorTexture.resize(width, height);
                        this._depthStencilTexture.resize(width, height);
                    }
                }
                _onWebGLContextLost(event) {
                    debug_5.warnID(11000);
                    debug_5.warn(event);
                    // 2020.9.3: `preventDefault` is not available on some platforms
                    // event.preventDefault();
                }
            };
            exports_67("WebGL2Swapchain", WebGL2Swapchain);
        }
    };
});
/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */
System.register("engine/cocos/core/gfx/webgl2/webgl2-device", ["engine/cocos/core/gfx/base/device", "engine/cocos/core/gfx/base/states/sampler", "engine/cocos/core/gfx/webgl2/webgl2-descriptor-set", "engine/cocos/core/gfx/webgl2/webgl2-buffer", "engine/cocos/core/gfx/webgl2/webgl2-command-buffer", "engine/cocos/core/gfx/webgl2/webgl2-framebuffer", "engine/cocos/core/gfx/webgl2/webgl2-input-assembler", "engine/cocos/core/gfx/webgl2/webgl2-descriptor-set-layout", "engine/cocos/core/gfx/webgl2/webgl2-pipeline-layout", "engine/cocos/core/gfx/webgl2/webgl2-pipeline-state", "engine/cocos/core/gfx/webgl2/webgl2-primary-command-buffer", "engine/cocos/core/gfx/webgl2/webgl2-queue", "engine/cocos/core/gfx/webgl2/webgl2-render-pass", "engine/cocos/core/gfx/webgl2/states/webgl2-sampler", "engine/cocos/core/gfx/webgl2/webgl2-shader", "engine/cocos/core/gfx/webgl2/webgl2-swapchain", "engine/cocos/core/gfx/webgl2/webgl2-texture", "engine/cocos/core/gfx/base/define", "engine/cocos/core/gfx/webgl2/webgl2-commands", "engine/cocos/core/gfx/base/states/global-barrier", "engine/cocos/core/gfx/base/states/texture-barrier", "engine/cocos/core/platform/debug", "engine/cocos/core/gfx/webgl2/webgl2-define"], function (exports_68, context_68) {
    "use strict";
    var device_2, sampler_4, webgl2_descriptor_set_1, webgl2_buffer_1, webgl2_command_buffer_2, webgl2_framebuffer_1, webgl2_input_assembler_1, webgl2_descriptor_set_layout_1, webgl2_pipeline_layout_1, webgl2_pipeline_state_1, webgl2_primary_command_buffer_1, webgl2_queue_1, webgl2_render_pass_1, webgl2_sampler_1, webgl2_shader_1, webgl2_swapchain_1, webgl2_texture_2, define_38, webgl2_commands_10, global_barrier_2, texture_barrier_2, debug_6, webgl2_define_11, WebGL2Device;
    var __moduleName = context_68 && context_68.id;
    return {
        setters: [
            function (device_2_1) {
                device_2 = device_2_1;
            },
            function (sampler_4_1) {
                sampler_4 = sampler_4_1;
            },
            function (webgl2_descriptor_set_1_1) {
                webgl2_descriptor_set_1 = webgl2_descriptor_set_1_1;
            },
            function (webgl2_buffer_1_1) {
                webgl2_buffer_1 = webgl2_buffer_1_1;
            },
            function (webgl2_command_buffer_2_1) {
                webgl2_command_buffer_2 = webgl2_command_buffer_2_1;
            },
            function (webgl2_framebuffer_1_1) {
                webgl2_framebuffer_1 = webgl2_framebuffer_1_1;
            },
            function (webgl2_input_assembler_1_1) {
                webgl2_input_assembler_1 = webgl2_input_assembler_1_1;
            },
            function (webgl2_descriptor_set_layout_1_1) {
                webgl2_descriptor_set_layout_1 = webgl2_descriptor_set_layout_1_1;
            },
            function (webgl2_pipeline_layout_1_1) {
                webgl2_pipeline_layout_1 = webgl2_pipeline_layout_1_1;
            },
            function (webgl2_pipeline_state_1_1) {
                webgl2_pipeline_state_1 = webgl2_pipeline_state_1_1;
            },
            function (webgl2_primary_command_buffer_1_1) {
                webgl2_primary_command_buffer_1 = webgl2_primary_command_buffer_1_1;
            },
            function (webgl2_queue_1_1) {
                webgl2_queue_1 = webgl2_queue_1_1;
            },
            function (webgl2_render_pass_1_1) {
                webgl2_render_pass_1 = webgl2_render_pass_1_1;
            },
            function (webgl2_sampler_1_1) {
                webgl2_sampler_1 = webgl2_sampler_1_1;
            },
            function (webgl2_shader_1_1) {
                webgl2_shader_1 = webgl2_shader_1_1;
            },
            function (webgl2_swapchain_1_1) {
                webgl2_swapchain_1 = webgl2_swapchain_1_1;
            },
            function (webgl2_texture_2_1) {
                webgl2_texture_2 = webgl2_texture_2_1;
            },
            function (define_38_1) {
                define_38 = define_38_1;
            },
            function (webgl2_commands_10_1) {
                webgl2_commands_10 = webgl2_commands_10_1;
            },
            function (global_barrier_2_1) {
                global_barrier_2 = global_barrier_2_1;
            },
            function (texture_barrier_2_1) {
                texture_barrier_2 = texture_barrier_2_1;
            },
            function (debug_6_1) {
                debug_6 = debug_6_1;
            },
            function (webgl2_define_11_1) {
                webgl2_define_11 = webgl2_define_11_1;
            }
        ],
        execute: function () {
            WebGL2Device = class WebGL2Device extends device_2.Device {
                constructor() {
                    super(...arguments);
                    this._swapchain = null;
                }
                get gl() {
                    return this._swapchain.gl;
                }
                get extensions() {
                    return this._swapchain.extensions;
                }
                get stateCache() {
                    return this._swapchain.stateCache;
                }
                get nullTex2D() {
                    return this._swapchain.nullTex2D;
                }
                get nullTexCube() {
                    return this._swapchain.nullTexCube;
                }
                initialize(info) {
                    webgl2_define_11.WebGL2DeviceManager.setInstance(this);
                    this._gfxAPI = define_38.API.WEBGL2;
                    this._bindingMappingInfo = info.bindingMappingInfo;
                    if (!this._bindingMappingInfo.bufferOffsets.length)
                        this._bindingMappingInfo.bufferOffsets.push(0);
                    if (!this._bindingMappingInfo.samplerOffsets.length)
                        this._bindingMappingInfo.samplerOffsets.push(0);
                    let gl = null;
                    try {
                        gl = document.createElement('canvas').getContext('webgl2');
                    }
                    catch (err) {
                        console.error(err);
                    }
                    if (!gl) {
                        console.error('This device does not support WebGL.');
                        return false;
                    }
                    // create queue
                    this._queue = this.createQueue(new define_38.QueueInfo(define_38.QueueType.GRAPHICS));
                    this._cmdBuff = this.createCommandBuffer(new define_38.CommandBufferInfo(this._queue));
                    this._caps.maxVertexAttributes = gl.getParameter(gl.MAX_VERTEX_ATTRIBS);
                    this._caps.maxVertexUniformVectors = gl.getParameter(gl.MAX_VERTEX_UNIFORM_VECTORS);
                    this._caps.maxFragmentUniformVectors = gl.getParameter(gl.MAX_FRAGMENT_UNIFORM_VECTORS);
                    this._caps.maxTextureUnits = gl.getParameter(gl.MAX_TEXTURE_IMAGE_UNITS);
                    this._caps.maxVertexTextureUnits = gl.getParameter(gl.MAX_VERTEX_TEXTURE_IMAGE_UNITS);
                    this._caps.maxUniformBufferBindings = gl.getParameter(gl.MAX_UNIFORM_BUFFER_BINDINGS);
                    this._caps.maxUniformBlockSize = gl.getParameter(gl.MAX_UNIFORM_BLOCK_SIZE);
                    this._caps.maxTextureSize = gl.getParameter(gl.MAX_TEXTURE_SIZE);
                    this._caps.maxCubeMapTextureSize = gl.getParameter(gl.MAX_CUBE_MAP_TEXTURE_SIZE);
                    this._caps.uboOffsetAlignment = gl.getParameter(gl.UNIFORM_BUFFER_OFFSET_ALIGNMENT);
                    const extensions = gl.getSupportedExtensions();
                    let extStr = '';
                    if (extensions) {
                        for (const ext of extensions) {
                            extStr += `${ext} `;
                        }
                    }
                    const exts = webgl2_swapchain_1.getExtensions(gl);
                    if (exts.WEBGL_debug_renderer_info) {
                        this._renderer = gl.getParameter(exts.WEBGL_debug_renderer_info.UNMASKED_RENDERER_WEBGL);
                        this._vendor = gl.getParameter(exts.WEBGL_debug_renderer_info.UNMASKED_VENDOR_WEBGL);
                    }
                    else {
                        this._renderer = gl.getParameter(gl.RENDERER);
                        this._vendor = gl.getParameter(gl.VENDOR);
                    }
                    const version = gl.getParameter(gl.VERSION);
                    this._features.fill(false);
                    this._features[define_38.Feature.TEXTURE_FLOAT] = true;
                    this._features[define_38.Feature.TEXTURE_HALF_FLOAT] = true;
                    this._features[define_38.Feature.FORMAT_R11G11B10F] = true;
                    this._features[define_38.Feature.FORMAT_SRGB] = true;
                    this._features[define_38.Feature.FORMAT_RGB8] = true;
                    this._features[define_38.Feature.ELEMENT_INDEX_UINT] = true;
                    this._features[define_38.Feature.INSTANCED_ARRAYS] = true;
                    this._features[define_38.Feature.MULTIPLE_RENDER_TARGETS] = true;
                    this._features[define_38.Feature.BLEND_MINMAX] = true;
                    if (exts.EXT_color_buffer_float) {
                        this._features[define_38.Feature.COLOR_FLOAT] = true;
                        this._features[define_38.Feature.COLOR_HALF_FLOAT] = true;
                    }
                    if (exts.OES_texture_float_linear) {
                        this._features[define_38.Feature.TEXTURE_FLOAT_LINEAR] = true;
                    }
                    if (exts.OES_texture_half_float_linear) {
                        this._features[define_38.Feature.TEXTURE_HALF_FLOAT_LINEAR] = true;
                    }
                    let compressedFormat = '';
                    if (exts.WEBGL_compressed_texture_etc1) {
                        this._features[define_38.Feature.FORMAT_ETC1] = true;
                        compressedFormat += 'etc1 ';
                    }
                    if (exts.WEBGL_compressed_texture_etc) {
                        this._features[define_38.Feature.FORMAT_ETC2] = true;
                        compressedFormat += 'etc2 ';
                    }
                    if (exts.WEBGL_compressed_texture_s3tc) {
                        this._features[define_38.Feature.FORMAT_DXT] = true;
                        compressedFormat += 'dxt ';
                    }
                    if (exts.WEBGL_compressed_texture_pvrtc) {
                        this._features[define_38.Feature.FORMAT_PVRTC] = true;
                        compressedFormat += 'pvrtc ';
                    }
                    if (exts.WEBGL_compressed_texture_astc) {
                        this._features[define_38.Feature.FORMAT_ASTC] = true;
                        compressedFormat += 'astc ';
                    }
                    debug_6.debug('WebGL2 device initialized.');
                    debug_6.debug(`RENDERER: ${this._renderer}`);
                    debug_6.debug(`VENDOR: ${this._vendor}`);
                    debug_6.debug(`VERSION: ${version}`);
                    debug_6.debug(`COMPRESSED_FORMAT: ${compressedFormat}`);
                    debug_6.debug(`EXTENSIONS: ${extStr}`);
                    return true;
                }
                destroy() {
                    if (this._queue) {
                        this._queue.destroy();
                        this._queue = null;
                    }
                    if (this._cmdBuff) {
                        this._cmdBuff.destroy();
                        this._cmdBuff = null;
                    }
                    const it = this._samplers.values();
                    let res = it.next();
                    while (!res.done) {
                        res.value.destroy();
                        res = it.next();
                    }
                    this._swapchain = null;
                }
                flushCommands(cmdBuffs) { }
                acquire(swapchains) { }
                present() {
                    const queue = this._queue;
                    this._numDrawCalls = queue.numDrawCalls;
                    this._numInstances = queue.numInstances;
                    this._numTris = queue.numTris;
                    queue.clear();
                }
                createCommandBuffer(info) {
                    // const Ctor = WebGLCommandBuffer; // opt to instant invocation
                    const Ctor = info.type === define_38.CommandBufferType.PRIMARY ? webgl2_primary_command_buffer_1.WebGL2PrimaryCommandBuffer : webgl2_command_buffer_2.WebGL2CommandBuffer;
                    const cmdBuff = new Ctor();
                    cmdBuff.initialize(info);
                    return cmdBuff;
                }
                createSwapchain(info) {
                    const swapchain = new webgl2_swapchain_1.WebGL2Swapchain();
                    this._swapchain = swapchain;
                    swapchain.initialize(info);
                    return swapchain;
                }
                createBuffer(info) {
                    const buffer = new webgl2_buffer_1.WebGL2Buffer();
                    buffer.initialize(info);
                    return buffer;
                }
                createTexture(info) {
                    const texture = new webgl2_texture_2.WebGL2Texture();
                    texture.initialize(info);
                    return texture;
                }
                createDescriptorSet(info) {
                    const descriptorSet = new webgl2_descriptor_set_1.WebGL2DescriptorSet();
                    descriptorSet.initialize(info);
                    return descriptorSet;
                }
                createShader(info) {
                    const shader = new webgl2_shader_1.WebGL2Shader();
                    shader.initialize(info);
                    return shader;
                }
                createInputAssembler(info) {
                    const inputAssembler = new webgl2_input_assembler_1.WebGL2InputAssembler();
                    inputAssembler.initialize(info);
                    return inputAssembler;
                }
                createRenderPass(info) {
                    const renderPass = new webgl2_render_pass_1.WebGL2RenderPass();
                    renderPass.initialize(info);
                    return renderPass;
                }
                createFramebuffer(info) {
                    const framebuffer = new webgl2_framebuffer_1.WebGL2Framebuffer();
                    framebuffer.initialize(info);
                    return framebuffer;
                }
                createDescriptorSetLayout(info) {
                    const descriptorSetLayout = new webgl2_descriptor_set_layout_1.WebGL2DescriptorSetLayout();
                    descriptorSetLayout.initialize(info);
                    return descriptorSetLayout;
                }
                createPipelineLayout(info) {
                    const pipelineLayout = new webgl2_pipeline_layout_1.WebGL2PipelineLayout();
                    pipelineLayout.initialize(info);
                    return pipelineLayout;
                }
                createPipelineState(info) {
                    const pipelineState = new webgl2_pipeline_state_1.WebGL2PipelineState();
                    pipelineState.initialize(info);
                    return pipelineState;
                }
                createQueue(info) {
                    const queue = new webgl2_queue_1.WebGL2Queue();
                    queue.initialize(info);
                    return queue;
                }
                getSampler(info) {
                    const hash = sampler_4.Sampler.computeHash(info);
                    if (!this._samplers.has(hash)) {
                        this._samplers.set(hash, new webgl2_sampler_1.WebGL2Sampler(info, hash));
                    }
                    return this._samplers.get(hash);
                }
                getGlobalBarrier(info) {
                    const hash = global_barrier_2.GlobalBarrier.computeHash(info);
                    if (!this._globalBarriers.has(hash)) {
                        this._globalBarriers.set(hash, new global_barrier_2.GlobalBarrier(info, hash));
                    }
                    return this._globalBarriers.get(hash);
                }
                getTextureBarrier(info) {
                    const hash = texture_barrier_2.TextureBarrier.computeHash(info);
                    if (!this._textureBarriers.has(hash)) {
                        this._textureBarriers.set(hash, new texture_barrier_2.TextureBarrier(info, hash));
                    }
                    return this._textureBarriers.get(hash);
                }
                copyBuffersToTexture(buffers, texture, regions) {
                    webgl2_commands_10.WebGL2CmdFuncCopyBuffersToTexture(this, buffers, texture.gpuTexture, regions);
                }
                copyTextureToBuffers(texture, buffers, regions) {
                    webgl2_commands_10.WebGL2CmdFuncCopyTextureToBuffers(this, texture.gpuTexture, buffers, regions);
                }
                copyTexImagesToTexture(texImages, texture, regions) {
                    webgl2_commands_10.WebGL2CmdFuncCopyTexImagesToTexture(this, texImages, texture.gpuTexture, regions);
                }
            };
            exports_68("WebGL2Device", WebGL2Device);
        }
    };
});
System.register("gfx-test-case/engine/engine", ["engine/cocos/core/gfx/webgl/webgl-device", "engine/cocos/core/gfx/webgl2/webgl2-device"], function (exports_69, context_69) {
    "use strict";
    var __moduleName = context_69 && context_69.id;
    function exportStar_1(m) {
        var exports = {};
        for (var n in m) {
            if (n !== "default") exports[n] = m[n];
        }
        exports_69(exports);
    }
    return {
        setters: [
            function (webgl_device_1_1) {
                exportStar_1(webgl_device_1_1);
            },
            function (webgl2_device_1_1) {
                exportStar_1(webgl2_device_1_1);
            }
        ],
        execute: function () {
        }
    };
});
System.register("gfx-test-case/engine/test-base", [], function (exports_70, context_70) {
    "use strict";
    var TestBase;
    var __moduleName = context_70 && context_70.id;
    return {
        setters: [],
        execute: function () {
            TestBase = class TestBase {
                initialize() {
                    this.onInit();
                }
                resize(swapchain) {
                    this.onResize(swapchain);
                }
                tick() {
                    this.onTick();
                }
                destroy() {
                    this.onDestroy();
                }
            };
            exports_70("TestBase", TestBase);
        }
    };
});
System.register("gfx-test-case/engine/index", ["gfx-test-case/engine/engine", "gfx-test-case/engine/test-base"], function (exports_71, context_71) {
    "use strict";
    var __moduleName = context_71 && context_71.id;
    function exportStar_2(m) {
        var exports = {};
        for (var n in m) {
            if (n !== "default") exports[n] = m[n];
        }
        exports_71(exports);
    }
    return {
        setters: [
            function (engine_1_1) {
                exportStar_2(engine_1_1);
            },
            function (test_base_1_1) {
                exportStar_2(test_base_1_1);
            }
        ],
        execute: function () {
        }
    };
});
//# sourceMappingURL=cocos.js.map