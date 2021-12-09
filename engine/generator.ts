import {
    Attribute, ShaderStageFlagBit, Uniform, UniformBlock, FormatInfos,
    API, UniformSamplerTexture,
} from 'gfx/base/define';
import { DescriptorSetLayout } from 'gfx/base/descriptor-set-layout';
import { IShaderVaryings, TypeInfos } from './chassis';
import { IStandardShaderSource } from './test-base';

export class SourceGenerator {
    private _genShaderHeaders: (source: IStandardShaderSource) => void;
    private _genShaderVBO: (attribute: Attribute) =>  string;
    private _genShaderUBO: (block: UniformBlock) => string;
    private _genShaderSampler: (sampler: UniformSamplerTexture) => string;
    private _genShaderVarying: (varying: IShaderVaryings, loc: number, source: IStandardShaderSource) => void;
    private _genShaderBody: (vert: string, frag: string, source: IStandardShaderSource) => void;

    constructor (api: API) {
        switch (api) {
        case API.WEBGL:
        case API.GLES2:
            this._genShaderHeaders = this._genShaderHeaders1;
            this._genShaderVBO = this._genShaderVBO1;
            this._genShaderUBO = this._genShaderUBO1;
            this._genShaderSampler = this._genShaderSampler1;
            this._genShaderVarying = this._genShaderVarying1;
            this._genShaderBody = this._genShaderBody1;
            break;
        case API.WEBGL2:
        case API.GLES3:
            this._genShaderHeaders = this._genShaderHeaders3;
            this._genShaderVBO = this._genShaderVBO3;
            this._genShaderUBO = this._genShaderUBO3;
            this._genShaderVarying = this._genShaderVarying4;
            this._genShaderSampler = this._genShaderSampler3;
            this._genShaderBody = this._genShaderBody3;
            break;
        case API.METAL:
        case API.VULKAN:
        default:
            this._genShaderHeaders = this._genShaderHeaders4;
            this._genShaderVBO = this._genShaderVBO4;
            this._genShaderUBO = this._genShaderUBO4;
            this._genShaderVarying = this._genShaderVarying4;
            this._genShaderSampler = this._genShaderSampler4;
            this._genShaderBody = this._genShaderBody4;
            break;
        }
    }

    public genShaderSource (attributes: Attribute[], blocks: UniformBlock[],
        samplerTextures: UniformSamplerTexture[], varyings: IShaderVaryings[], vert: string, frag: string,
        descriptorSetLayouts: DescriptorSetLayout[]): IStandardShaderSource {
        const source: IStandardShaderSource = { vert: '', frag: '' };

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
        this._genShaderBody(vert, frag, source);
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

    private _genShaderVBO1 (attribute: Attribute) : string {
        return `attribute vec${FormatInfos[attribute.format].count} ${attribute.name};`;
    }
    private _genShaderVBO3 (attribute: Attribute) : string {
        return `in vec${FormatInfos[attribute.format].count} ${attribute.name};`;
    }
    private _genShaderVBO4 (attribute: Attribute) : string {
        return `layout(binding = ${attribute.stream}, location = ${attribute.location})`
    + ` in vec${FormatInfos[attribute.format].count} ${attribute.name};`;
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
            source += `layout(std140) uniform ${block.name} {`;
            for (const member of block.members) {
                source += `    ${this._genShaderVaryingNumObj(member)}\n`;
            }
            source += `}\n`;
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
            source += `layout(set = ${block.set}, binding = ${block.binding}) uniform ${block.name} {`;
            for (const member of block.members) {
                source += `    ${this._genShaderVaryingNumObj(member)}\n`;
            }
            source += `}\n`;
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
        source.vert += `varying out ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
        source.frag += `varying in ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
    }
    private _genShaderVarying3 (varying: IShaderVaryings, loc: number, source: IStandardShaderSource) : void {
        source.vert += `out ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
        source.frag += `in ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
    }
    private _genShaderVarying4 (varying: IShaderVaryings, loc: number, source: IStandardShaderSource) : void {
        source.vert += `layout (location = ${loc}) out ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
        source.frag += `layout (location = ${loc}) in ${TypeInfos[varying.type].glslName} ${varying.name};\n`;
    }
    private _genShaderBody1 (vert: string, frag: string, source: IStandardShaderSource): void {
        source.vert += `${vert}
            void main() {
                gl_Position = vert();
            }`;
        source.frag += `${frag}
            void main() {
                gl_FragColor = u_color;
            }`;
    }
    private _genShaderBody3 (vert: string, frag: string, source: IStandardShaderSource): void {
        source.vert += `${vert}
            void main() {
                gl_Position = vert();
            }`;
        source.frag += `${frag}
            out vec4 o_color;
            void main() {
                o_color = u_color;
            }`;
    }
    private _genShaderBody4 (vert: string, frag: string, source: IStandardShaderSource): void {
        source.vert += `${vert}
            void main() {
                gl_Position = vert();
            }`;
        source.frag += `${frag}
            layout (location = 0) out vec4 o_color;
            void main() {
                o_color = u_color;
            }`;
    }
}
