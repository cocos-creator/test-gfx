import { Attribute, PrimitiveMode } from 'gfx/base/define';

export interface IGeometry {
    positions: number[];
    normals?: number[];
    uvs?: number[];
    tangents?: number[];
    colors?: number[];
    attributes?: Attribute[];
    customAttributes?: {
        attr: Attribute,
        values: number[],
    }[];

    boundingRadius?: number;
    minPos?: {
        x: number;
        y: number;
        z: number;
    };
    maxPos?: {
        x: number;
        y: number;
        z: number;
    };
    indices?: number[];
    primitiveMode?: PrimitiveMode;
    doubleSided?: boolean;
}

export interface IGeometryOptions {
    /**
     * @en
     * Whether to include normal. Default to true.
     * @zh
     * 是否包含法线。默认为true。
     */
    includeNormal: boolean;

    /**
     * @en
     * Whether to include uv. Default to true.
     * @zh
     * 是否包含UV。默认为true。
     */
    includeUV: boolean;
}
