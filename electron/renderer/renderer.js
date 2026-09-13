// renderer.js
// 渲染进程逻辑
const log = (msg) => {
  const el = document.getElementById('log');
  el.textContent += `[${new Date().toLocaleTimeString()}] ${msg}\n`;
  el.scrollTop = el.scrollHeight;
};

const $ = (id) => document.getElementById(id);

// 初始化：显示极域目录
try {
  const dir = window.jiyu.findStudentMainDir();
  $('dirInfo').textContent = dir ? `极域目录：${dir}` : '极域目录：未检测（StudentMain.exe 未运行）';
} catch (e) {
  log('初始化失败：' + e.message);
}

$('btnUnlock').addEventListener('click', () => {
  try {
    const r = window.jiyu.unlockDCS();
    log(`解锁键盘/鼠标：DCS_EnableKeys 返回 ${r}${r === 0 ? '（成功）' : ''}`);
  } catch (e) { log('解锁失败：' + e.message); }
});

$('btnInject').addEventListener('click', () => {
  try {
    const ok = window.jiyu.injectToStudentMain();
    log(`注入反监视 DLL：${ok ? '成功' : '失败（需管理员权限）'}`);
  } catch (e) { log('注入失败：' + e.message); }
});

$('btnReadPwd').addEventListener('click', () => {
  try {
    const pw = window.jiyu.readKnock1Password();
    $('pwdInfo').textContent = pw ? `解锁密码：${pw}` : '未读取到密码';
    log(pw ? `Knock1 密码：${pw}` : 'Knock1 密码读取失败（需管理员权限）');
  } catch (e) { log('读密码失败：' + e.message); }
});

$('btnFileFilter').addEventListener('click', () => {
  const ok = window.jiyu.unloadFileFilter();
  log(`卸载 TDFileFilter：${ok ? '成功' : '失败/不存在'}`);
});

$('btnNetFilter').addEventListener('click', () => {
  const ok = window.jiyu.unloadNetFilter();
  log(`卸载 TDNetFilter：${ok ? '成功' : '失败/不存在'}`);
});

const getTarget = () => ({
  ip: $('ip').value.trim(),
  port: parseInt($('port').value, 10) || 0,
  payload: $('payload').value,
});

$('btnMsg').addEventListener('click', () => {
  const { ip, port, payload } = getTarget();
  const ok = window.jiyu.sendMessage(ip, port, payload);
  log(`发消息到 ${ip}:${port}：${ok ? '已发送' : '发送失败'}`);
});

$('btnShutdown').addEventListener('click', () => {
  const { ip, port, payload } = getTarget();
  const ok = window.jiyu.sendShutdown(ip, port, 10, payload);
  log(`关机指令到 ${ip}:${port}：${ok ? '已发送' : '发送失败'}`);
});

$('btnReboot').addEventListener('click', () => {
  const { ip, port, payload } = getTarget();
  const ok = window.jiyu.sendReboot(ip, port, 10, payload);
  log(`重启指令到 ${ip}:${port}：${ok ? '已发送' : '发送失败'}`);
});

$('btnExec').addEventListener('click', () => {
  const { ip, port, payload } = getTarget();
  const ok = window.jiyu.sendExecCommand(ip, port, payload);
  log(`执行命令到 ${ip}:${port}：${ok ? '已发送' : '发送失败'}`);
});
