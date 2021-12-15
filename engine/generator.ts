import {
    Attribute, ShaderStageFlagBit, Uniform, UniformBlock, FormatInfos,
    API, UniformSamplerTexture,
} from 'gfx/base/define';
import { DescriptorSetLayout } from 'gfx/base/descriptor-set-layout';
import { IShaderVaryings, TypeInfos, IShaderExtension, IShaderExtensionType, IShaderExtensionTypeName } from './chassis';
import { IStandardShaderSource } from './test-base';

export class SourceGenerator {
    private _shaderFragOut = `
    struct FragColor {
        vec4 fragColor0;
        vec4 fragColor1;
        vec4 fragColor2;
        vec4 fragColor3;
    };`;

    private _genShaderHeaders: (source: IStandardShaderSource) => void;
    private _genShaderExtension: (extension: IShaderExtension) => string;
    private _genShaderVBO: (attribute: Attribute) =>  string;
    private _genShaderUBO: (block: UniformBlock) => string;
    private _genShaderSampler: (sampler: UniformSamplerTexture) => string;
    private _genShaderVarying: (varying: IShaderVaryings, loc: number, source: IStandardShaderSource) => void;
    private _genShaderBody: (vert: string, frag: string, source: IStandardShaderSource, attachment: number) => void;

    constructor (api: API) {
        switch (api) {
        case API.WEBGL:
        case API.GLES2:
            this._genShaderHeaders = this._genShaderHeaders1;
            this._genShaderExtension = this._genShaderExtension1;
            this._genShaderVBO = this._genShaderVBO1;
            this._genShaderUBO = this._genShaderUBO1;
            this._genShaderSampler = this._genShaderSampler1;
            this._genShaderVarying = this._genShaderVarying1;
            this._genShaderBody = this._genShaderBody1;
            break;
        case API.WEBGL2:
        case API.GLES3:
            this._genShaderHeaders = this._genShaderHeaders3;
            this._genShaderExtension = this._genShaderExtension1;
            this._genShaderVBO = this._genShaderVBO3;
            this._genShaderUBO = this._genShaderUBO3;
            this._genShaderVarying = this._genShaderVarying3;
            this._genShaderSampler = this._genShaderSampler3;
            this._genShaderBody = this._genShaderBody3;
            break;
        case API.METAL:
        case API.VULKAN:
        default:
            this._genShaderHeaders = this._genShaderHeaders4;
            this._genShaderExtension = this._genShaderExtension1;
            this._genShaderVBO = this._genShaderVBO4;
            this._genShaderUBO = this._genShaderUBO4;
            this._genShaderVarying = this._genShaderVarying4;
            this._genShaderSampler = this._genShaderSampler4;
            this._genShaderBody = this._genShaderBody4;
            break;
        }
    }

    public genShaderSource (
        name: string,
        vert: string,
        frag: string,
        extensions: IShaderExtension[],
        attributes: Attribute[],
        blocks: UniformBlock[],
        samplerTextures: UniformSamplerTexture[],
        varyings: IShaderVaryings[],
        attachments: number,
        descriptorSetLayouts: DescriptorSetLayout[],
    ): IStandardShaderSource {
        const source: IStandardShaderSource = { vert: '', frag: '' };

        // Extensions
        for (const extension of extensions) {
            source.vert += this._genShaderExtension(extension);
            source.frag += this._genShaderExtension(extension);
        }

        // header and vertex attribute
        this._genShaderHeaders(source);
        for (const attribute of attributes || []) {
            source.vert += this._genShaderVBO(attribute);
        }

        // uniform buffer
        let stageFlag = ShaderStageFlagBit.NONE;
        for (const block of blocks) {
            stageFlag = descriptorSetLayouts[block.set].bindings[block.binding].stageFlags;
            if (stageFlag & ShaderStageFlagBit.VERTEX) source.vert += this._genShaderUBO(block);
            if (stageFlag & ShaderStageFlagBit.FRAGMENT) source.frag += this._genShaderUBO(block);
        }

        // sampler
        for (const sampler of samplerTextures) {
            stageFlag = descriptorSetLayouts[sampler.set].bindings[sampler.binding].stageFlags;
            if (stageFlag & ShaderStageFlagBit.VERTEX) source.vert += this._genShaderSampler(sampler);
            if (stageFlag & ShaderStageFlagBit.FRAGMENT) source.frag += this._genShaderSampler(sampler);
        }

        // varyings
        for (let i = 0; i < varyings.length; i++) {
            this._genShaderVarying(varyings[i], i, source);
        }

        // functions
        this._genShaderBody(vert, frag, source, attachments);
        return source;
    }

    private _genShaderHeaders1 (source: IStandardShaderSource) : void {
        source.vert += ``;
        source.frag += `precision mediump float;\n`;
    }
    private _genShaderHeaders3 (source: IStandardShaderSource) : void {
        source.vert += ``;
        source.frag += `precision mediump float;\n`;
    }
    private _genShaderHeaders4 (source: IStandardShaderSource) : void {
        source.vert += `precision highp float;\n`;
        source.frag += `precision highp float;\n`;
    }

    private _genShaderExtension1 (extension: IShaderExtension): string {
        return `#extension ${extension.name}: ${IShaderExtensionTypeName[extension.type]}\n`;
    }

    private _genShaderVBO1 (attribute: Attribute) : string {
        return `attribute vec${FormatInfos[attribute.format].count} ${attribute.name};\n`;
    }
    private _genShaderVBO3 (attribute: Attribute) : string {
        return `in vec${FormatInfos[attribute.format].count} ${attribute.name};\n`;
    }
    private _genShaderVBO4 (attribute: Attribute) : string {
        return `layout(binding = ${attribute.stream}, location = ${attribute.location})`
    + ` in vec${FormatInfos[attribute.format].count} ${attribute.name};\n`;
    }

    private _genShaderVaryingNumObj (obj: Uniform | UniformSamplerTexture): string {
        return (obj.count || 1) === 1
            ? `${TypeInfos[obj.type].glslName} ${obj.name};`
            : `${TypeInfos[obj.type].glslName} ${obj.name}[${obj.count}];`;
    }

    private _genShaderUBO1 (block: UniformBlock): string {
        let source = '';
        for (const member of block.members) {
            source += `uniform ${this._genShaderVaryingNumObj(member)}\n`;
        }
        return source;
    }
    private _genShaderUBO3 (block: UniformBlock) : string {
        let source = '';
        if (block.members.length === 1) {
            const member = block.members[0];
            source += `layout(std140) uniform ${block.name} { ${this._genShaderVaryingNumObj(member)} };\n`;
        } else {
            source += `layout(std140) uniform ${block.name} {\n`;
            for (const member of block.members) {
                source += `    ${this._genShaderVaryingNumObj(member)}\n`;
            }
            source += `};\n`;
        }
        return source;
    }
    private _genShaderUBO4 (block: UniformBlock) : string {
        let source = '';
        if (block.members.length === 1) {
            const member = block.members[0];
            source += `layout(set = ${block.set}, binding = ${block.binding})`
        + ` uniform ${block.name} { ${this._genShaderVaryingNumObj(member)} };\n`;
        } else {
            source += `layout(set = ${block.set}, binding = ${block.binding}) uniform ${block.name} {\n`;
            for (const member of block.members) {
                source += `    ${this._genShaderVaryingNumObj(member)}\n`;
            }
            source += `};\n`;
        }
        return source;
    }

    private _genShaderSampler1 (sampler: UniformSamplerTexture) : string {
        return `uniform ${this._genShaderVaryingNumObj(sampler)}\n`;
    }
    private _genShaderSampler3 (sampler: UniformSamplerTexture) : string {
        return `layout(std140) uniform ${this._genShaderVaryingNumObj(sampler)}\n`;
    }
    private _genShaderSampler4 (sampler: UniformSamplerTexture) : string {
        return `layout(set = ${sampler.set}, binding = ${sampler.binding}) uniform ${this._genShaderVaryingNumObj(sampler)}\n`;
    }

    private _genShaderVarying1 (varying: IShaderVaryings, loc: number, source: IStandardShaderSource) : void {
        source.vert += `varying ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
        source.frag += `varying ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
    }
    private _genShaderVarying3 (varying: IShaderVaryings, loc: number, source: IStandardShaderSource) : void {
        source.vert += `out ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
        source.frag += `in ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
    }
    private _genShaderVarying4 (varying: IShaderVaryings, loc: number, source: IStandardShaderSource) : void {
        source.vert += `layout (location = ${loc}) out ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
        source.frag += `layout (location = ${loc}) in ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
    }
    private _genShaderBody1 (vert: string, frag: string, source: IStandardShaderSource, attachments = 1): void {
        source.vert += `${vert}
            void main() {
                gl_Position = vert();
            }`;
        source.frag += `
            ${this._shaderFragOut}
            ${frag}
            void main() {
                FragColor o;
                frag(o);
            `;
        for (let i = 0; i < attachments; i++) {
            source.frag += `    gl_FragColor${i} = o.fragColor${i};\n`;
        }
        source.frag += `}`;
    }
    private _genShaderBody3 (vert: string, frag: string, source: IStandardShaderSource, attachments = 1): void {
        source.vert += `${vert}
            void main() {
                gl_Position = vert();
            }`;
        source.frag += `${this._shaderFragOut}\n${frag}\n`;
        for (let i = 0; i < attachments; i++) {
            source.frag += `out vec4 o_color${i};\n`;
        }
        source.frag += `
            void main() {
                FragColor o;
                frag(o);\n`;
        for (let i = 0; i < attachments; i++) {
            source.frag += `o_color${i} = o.fragColor${i};\n`;
        }
        source.frag += `}`;
    }
    private _genShaderBody4 (vert: string, frag: string, source: IStandardShaderSource, attachments = 1): void {
        source.vert += `${vert}
            void main() {
                gl_Position = vert();
            }`;
        source.frag += `${this._shaderFragOut}\n${frag}\n`;
        for (let i = 0; i < attachments; i++) {
            source.frag += `layout (location = ${i}) out vec4 o_color${i};\n`;
        }
        source.frag += `
            void main() {
                FragColor o;
                frag(o);\n`;
        for (let i = 0; i < attachments; i++) {
            source.frag += `o_color${i} = o.fragColor${i};\n`;
        }
        source.frag += `}`;
    }
}
