# .devcontainer/Dockerfile
FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=${USER_UID}

ENV TZ=Asia/Tokyo \
    LANG=ja_JP.UTF-8 \
    LANGUAGE=ja_JP:ja \
    LC_ALL=ja_JP.UTF-8

# 1) パッケージ + タイムゾーン/ロケール
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    sudo locales tzdata ca-certificates \
    build-essential clang make cmake pkg-config \
    gdb valgrind strace ltrace \
    git curl wget unzip zip \
    vim nano less \
    bash dash busybox-static \
    coreutils procps file \
    diffutils grep sed \
    libreadline-dev && \
    ln -fs /usr/share/zoneinfo/${TZ} /etc/localtime && \
    dpkg-reconfigure -f noninteractive tzdata && \
    sed -i 's/^# *ja_JP.UTF-8 UTF-8/ja_JP.UTF-8 UTF-8/' /etc/locale.gen && \
    locale-gen ja_JP.UTF-8 && \
    update-locale LANG=ja_JP.UTF-8 && \
    rm -rf /var/lib/apt/lists/*

# 2) ユーザ/グループ作成（UID/GID衝突があってもOK）— Here-Doc で安全に実行
RUN <<'SH'
set -eux

# group: 既存GIDならリネーム、なければ作成
if getent group "${USER_GID}" >/dev/null; then
  existing_group="$(getent group "${USER_GID}" | cut -d: -f1)"
  if [ "${existing_group}" != "${USERNAME}" ]; then
    groupmod -n "${USERNAME}" "${existing_group}"
  fi
else
  groupadd --gid "${USER_GID}" "${USERNAME}"
fi

# user: 既存UIDならリネーム/ホーム移動、なければ作成
if getent passwd "${USER_UID}" >/dev/null; then
  existing_user="$(getent passwd "${USER_UID}" | cut -d: -f1)"
  if [ "${existing_user}" != "${USERNAME}" ]; then
    usermod -l "${USERNAME}" -d "/home/${USERNAME}" -m "${existing_user}"
  fi
  usermod -g "${USER_GID}" "${USERNAME}"
else
  useradd -s /bin/bash --uid "${USER_UID}" --gid "${USER_GID}" -m "${USERNAME}"
fi

# sudoers
echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > "/etc/sudoers.d/90-${USERNAME}"
chmod 0440 "/etc/sudoers.d/90-${USERNAME}"

# Valgrind サプレッションの置き場（中身は後で差し替え可）
mkdir -p "/home/${USERNAME}/.valgrind"
touch "/home/${USERNAME}/.valgrind/valgrind-minishell.supp"
chown -R "${USERNAME}:${USERNAME}" "/home/${USERNAME}/.valgrind"
SH

WORKDIR /work
USER ${USERNAME}




