import { Color } from 'gfx/base/define';
import { TestBase } from '../test-base';

export class ClearScreen extends TestBase {
    private _clearColor = new Color(1, 0, 0, 1);

    public onTick () {
        const { accumulatedTime } = TestBase;
        this._clearColor.y = Math.abs(Math.sin(accumulatedTime));

        this._beginOnscreenPass(this._clearColor);
        this._endOnscreenPass();
        this._present();
    }
}
