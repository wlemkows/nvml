#!/usr/bin/env bash
#
# Copyright 2018, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#
#     * Neither the name of the copyright holder nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set -e

BOT_NAME="WeronikaLewandowska"
USER_NAME="wlemkows"
REPO_NAME="pmdk"

ORIGIN="https://${GITHUB_TOKEN}@github.com/${BOT_NAME}/${REPO_NAME}"
UPSTREAM="https://github.com/${USER_NAME}/${REPO_NAME}"
# master or stable-* branch
TARGET_BRANCH=${TRAVIS_BRANCH}
declare -A TARGET_BRANCHES=(["master"]="master" ["stable-1.2"]="v1.2" ["stable-1.3"]="v1.3" ["stable-1.4"]="v1.4" ["stable-1.5"]="v1.5")
VERSION=${TARGET_BRANCHES[$TARGET_BRANCH]}

if [ -z $VERSION ]; then
	echo "Target location for branch $TARGET_BRANCH is not defined."
	exit 1
fi

# Clone bot repo
git clone ${ORIGIN}
cd ${REPO_NAME}
git remote add upstream ${UPSTREAM}

git config --local user.name ${BOT_NAME}
git config --local user.email "taj5wero@gmail.com"

git checkout -B ${TARGET_BRANCH}
git remote update
git rebase upstream/${TARGET_BRANCH}

make doc
echo "----------------"
python --version
python3 --version
git branch
# Build & PR groff
git add -A
git commit -m "doc: automatic $TARGET_BRANCH docs update" && true
git push -f ${ORIGIN} ${TARGET_BRANCH}

# Makes pull request.
# When there is already an open PR or there are no changes an error is thrown, which we ignore.
hub pull-request -f -b ${USER_NAME}:${TARGET_BRANCH} -h ${BOT_NAME}:${TARGET_BRANCH} -m "doc: automatic $TARGET_BRANCH docs update" && true

git clean -dfx

# Copy man & PR web md
cd  ./doc
make web
cd ..

#mkdir ../web_manpages
mv ./doc/web_linux ../
mv ./doc/web_windows ../

# Checkout gh-pages and copy docs
git checkout -fb gh-pages upstream/gh-pages
git clean -dfx

cp -r  ../web_linux/* ./manpages/linux/${VERSION}/
cp -r  ../web_windows/* ./manpages/windows/${VERSION}/

# Add and push changes.
# git commit command may fail if there is nothing to commit.
# In that case we want to force push anyway (there might be open pull request with
# changes which were reverted).
git add -A
git commit -m "doc: automatic gh-pages docs update" && true
git push -f ${ORIGIN} gh-pages

hub pull-request -f -b ${USER_NAME}:gh-pages -h ${BOT_NAME}:gh-pages -m "doc: automatic gh-pages docs update" && true

exit 0
