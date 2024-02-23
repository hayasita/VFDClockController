
import { vfdControllerUI } from '../src/viewmodel/vmapp.js';

jest.mock('../src/viewmodel/vmapp.js', () => {
  const originalModule = jest.requireActual('../src/viewmodel/vmapp.js');
  return {
    __esModule: true,
    ...originalModule,
    vfdControllerUI: jest.fn().mockImplementation(function(initialSetting) {
      const originalInstance = new originalModule.vfdControllerUI(initialSetting);
      originalInstance.model.websocketSend = jest.fn().mockReturnValue('Mocked websocketSend');
      originalInstance.getTitle = jest.fn().mockReturnValue('Mocked method 1');
      return originalInstance;
    })
  };
});

describe('vfdControllerUI', () => {
  it('should use mocked websocketSend when calling submitWebsocket', () => {
    const vmUI = new vfdControllerUI('{"title":"VFD Clock Controler"}');
    expect(vmUI.submitWebsocket()).toEqual('Mocked websocketSend');
    expect(vmUI.getTitle()).toEqual('Mocked method 1');

    expect(vmUI.model.setSettingJsonItem("Takahiro","name")).toMatch("Takahiro");
    expect(vmUI.model.setSettingJsonItem(98,"brDig",2)).toBe(98);
//    expect(vmUI.dispBrDigSubmit(2,98)).toBe(98);
 
  });
});
