// preload.js
// 预加载脚本：把 native addon 接口安全暴露给渲染进程。
// 打包后从 resources 目录加载编译产物，开发模式从 build/ 目录加载。
const { contextBridge } = require('electron');
const path = require('path');
const fs = require('fs');

function resolvePath(prodName, devRel) {
  const prod = path.join(process.resourcesPath, prodName);
  if (fs.existsSync(prod)) return prod;
  return path.join(__dirname, '..', devRel);
}

// 定位编译产物
const addonPath = resolvePath('jiyu_native.node', path.join('build', 'Release', 'jiyu_native.node'));
const injectDllPath = resolvePath('jiyu_inject.dll', path.join('build', 'jiyu_inject.dll'));

const native = require(addonPath);

contextBridge.exposeInMainWorld('jiyu', {
  // 解锁键盘/鼠标（DCS_EnableKeys）
  unlockDCS: () => native.unlockDCS(),
  // 注入 DLL 到 StudentMain.exe（反监视 + 兜底解锁），自动使用打包/开发路径
  injectToStudentMain: () => native.injectToStudentMain(injectDllPath),
  // UDP 攻击（DMOC 协议）
  sendShutdown: (ip, port, delay, msg) => native.sendShutdown(ip, port, delay, msg),
  sendReboot: (ip, port, delay, msg) => native.sendReboot(ip, port, delay, msg),
  sendMessage: (ip, port, text) => native.sendMessage(ip, port, text),
  sendExecCommand: (ip, port, cmd) => native.sendExecCommand(ip, port, cmd),
  // Knock1 密码破解
  readKnock1Password: () => native.readKnock1Password(),
  // 驱动卸载
  unloadFileFilter: () => native.unloadFileFilter(),
  unloadNetFilter: () => native.unloadNetFilter(),
  // 定位极域安装目录
  findStudentMainDir: () => native.findStudentMainDir(),
});
