/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sig_setup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: takawagu <takawagu@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 12:50:38 by keitabe           #+#    #+#             */
/*   Updated: 2025/11/09 20:43:42 by takawagu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "signals.h"

volatile sig_atomic_t g_sig; // mainに置く

// REPL（readline呼び出し中）にCtrl-C（SIGINT）が来たら、
//フラグを立てる（g_sig = 1）
// 1文字の改行を端末に出す
//それ以外は一切しない。画面の整形・行の破棄はREPL側の処理に任せる。
//非同期安全性を守る（write(2)と単純代入のみ）。
static void	handler_int_rl(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "^C", 2);
	g_sig = 1;
}

static int	rl_sigint_hook(void)
{
	if (g_sig)
	{
		rl_replace_line("", 0);
		rl_done = 1;
	}
	return (0);
}

// readlineによるプロンプト待ちの最中に
// SIGINT（Ctrl-C）：REPL専用ハンドラ handler_int_rl を使い、readlineをEINTRで中断させる
// SIGQUIT（Ctrl-\）：無視にする（Bash体感に寄せる）
//これにより、Ctrl-Cで行を捨てて新しいPS1に遷移できる（実際の行破棄はREPL側で空文字返却により達成）。
void	sig_setup_readline(void)
{
	struct sigaction	sa;
	struct sigaction	ign;

	rl_catch_signals = 0;
	rl_event_hook = rl_sigint_hook;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler_int_rl;
	sigaction(SIGINT, &sa, NULL);
	sigemptyset(&ign.sa_mask);
	ign.sa_flags = 0;
	ign.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &ign, NULL);
}

//親シェルが子を実行・待機している短い区間だけ、親に届く SIGINT / SIGQUIT を無視に切り替える。
//端末（TTY）は前面プロセスグループに信号を投げるため、親も受けるが、親が無視することで子だけがデフォルト動作（終了など）になる。
// wait* が終わったらすぐに sig_setup_readline() に戻す。
void	sig_setup_parent_wait(void)
{
	struct sigaction	ign;

	sigemptyset(&ign.sa_mask);
	ign.sa_flags = 0;
	ign.sa_handler = SIG_IGN;
	sigaction(SIGINT, &ign, NULL);
	sigaction(SIGQUIT, &ign, NULL);
}

// fork() した子プロセスで、execve() の直前に、SIGINT / SIGQUIT をデフォルト挙動（SIG_DFL）に戻す。
//外部コマンドは普通のプロセスとしてCtrl-C/Ctrl-\ に反応できるようにする。
void	sig_setup_child_exec(void)
{
	struct sigaction	dfl;

	sigemptyset(&dfl.sa_mask);
	dfl.sa_flags = 0;
	dfl.sa_handler = SIG_DFL;
	sigaction(SIGINT, &dfl, NULL);
	sigaction(SIGQUIT, &dfl, NULL);
}
